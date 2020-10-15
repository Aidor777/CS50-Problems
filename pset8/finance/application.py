import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    if request.method == "GET":
        # Find all currently owned stocks by user
        user_id = session.get("user_id")
        stocks = db.execute("SELECT count, stock_symbol FROM stocks WHERE user_id = :user_id", user_id=user_id)
        if not stocks:
            stocks = []

        # For each stock, find its current value via the API, and add it to the total
        total = 0
        for stock in stocks:
            quote_data = lookup(stock["stock_symbol"])
            if not quote_data:
                return apology("Something went wrong", 500)

            stock["stock_price"] = quote_data["price"]
            stock["stock_name"] = quote_data["name"]
            stock["total"] = stock["stock_price"] * stock["count"]
            total += stock["total"]

        # Also get user's current cash
        rows = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=user_id)
        if len(rows) != 1 or not rows[0]["cash"]:
            return apology("something went wrong with the database", 500)
        total += rows[0]["cash"]

        return render_template("index.html", stocks=stocks, balance=rows[0]["cash"], total=total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "GET":
        return render_template("buy.html")

    else:
        symbol = request.form.get("symbol")
        # Ensure a symbol was submitted
        if not symbol:
            return apology("missing symbol", 400)

        shares = request.form.get("shares")
        # Ensure a proper number of shares was submitted
        if not shares:
            return apology("missing shares", 400)
        try:
            share_count = int(shares)
        except (TypeError):
            return apology("number of shares must be an integer", 400)
        if share_count < 1:
            return apology("invalid number of shares", 400)

        # Use lookup function to query API data about stocks
        quote_data = lookup(symbol)

        # Ensure the symbol was a valid stock market symbol
        if not quote_data:
            return apology("invalid symbol", 400)

        # Find account balance in DB
        user_id = session.get("user_id")
        rows = db.execute("SELECT cash FROM users WHERE id = :id", id=user_id)
        if len(rows) != 1 or not rows[0]["cash"]:
            return apology("something went wrong with the database", 500)

        # Check if the user has enough cash
        transaction_price = share_count * quote_data["price"]
        balance = rows[0]["cash"]
        if balance < transaction_price:
            return apology("can't afford", 400)
        remaining_cash = balance - transaction_price

        # Update the stock table as needed
        stock_symbol=quote_data["symbol"]
        rows = db.execute("SELECT count FROM stocks WHERE stock_symbol = :stock_symbol AND user_id = :user_id",
                stock_symbol=stock_symbol, user_id=user_id)
        if not rows:
            db.execute("INSERT INTO stocks (user_id, count, stock_symbol) " +
                    "VALUES (:user_id, :count, :stock_symbol)",
                    user_id=user_id, count=share_count, stock_symbol=stock_symbol)
        elif len(rows) == 1 and rows[0]["count"] != None:
            db.execute("UPDATE stocks SET count = :count WHERE user_id = :user_id AND stock_symbol = :stock_symbol",
                    user_id=user_id, count=share_count + rows[0]["count"], stock_symbol=stock_symbol)
        else:
            return apology("something went wrong with the database", 500)

        # Make the purchase in DB
        db.execute("INSERT INTO transactions (user_id, symbol, share_count, price) " +
                "VALUES (:user_id, :symbol, :share_count, :price)",
                user_id=user_id, symbol=stock_symbol, share_count=share_count, price=transaction_price)
        db.execute("UPDATE users SET cash = :cash WHERE id = :id", cash=remaining_cash, id=user_id)

        # Flash this message so that it is displayed after redirection
        flash("Bought!")

        # Redirect user to home page
        return redirect("/")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # Go get the transactions in DB
    user_id = session.get("user_id")
    transactions = db.execute("SELECT * FROM transactions WHERE user_id = :user_id", user_id=user_id)
    if not transactions:
        transactions = []

    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Flash this message so that it is displayed after redirection
        flash("Logged in successfully!")

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "GET":
        return render_template("quote.html")

    else:
        symbol = request.form.get("symbol")
        # Ensure a symbol was submitted
        if not symbol:
            return apology("missing symbol", 400)

        # Use lookup function to query API data about stocks
        quote_data = lookup(symbol)

        # Ensure the symbol was a valid stock market symbol
        if not quote_data:
            return apology("invalid symbol", 400)

        return render_template("quoted.html", quote_data=quote_data)


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        username = request.form.get("username")
        # Ensure username was submitted
        if not username:
            return apology("must provide username", 400)

        password = request.form.get("password")
        # Ensure password was submitted
        if not password:
            return apology("must provide password", 400)

        # Ensure password confirmation matches (also if confirmation is blank)
        elif password != request.form.get("confirmation"):
            return apology("password and confirmation must match", 400)

        # Query database for username to see if it exists or not
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=username)
        if len(rows) != 0:
            return apology("username already taken", 400)

        # Create new user in database
        db.execute("INSERT INTO users (username, hash) VALUES (:username, :hash)",
                username=username, hash=generate_password_hash(password))

        # Query database for the user to get its id
        id = db.execute("SELECT id FROM users WHERE username = :username", username=username)
        if len(rows) != 0:
            return apology("something went wrong with the database", 500)

        # Log the new user in directly
        session["user_id"] = id

        # Flash this message so that it is displayed after redirection
        flash("Registered successfully!")

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    if request.method == "GET":
        # Select possible stocks to be sold
        rows = db.execute("SELECT stock_symbol FROM stocks WHERE user_id = :user_id", user_id=session.get("user_id"))
        symbols = []
        for row in rows:
            symbols.append(row["stock_symbol"])

        return render_template("sell.html", symbols=symbols)

    else:
        # Check submitted form data
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("missing symbol", 400)

        shares = request.form.get("shares")
        if not shares:
            return apology("missing shares", 400)
        try:
            share_count = int(shares)
        except (TypeError):
            return apology("number of shares must be an integer", 400)
        if share_count < 1:
            return apology("invalid number of shares", 400)

        quote_data = lookup(symbol)

        # Ensure the symbol was a valid stock market symbol
        if not quote_data:
            return apology("invalid symbol", 400)

        # Make sure the user has enough shares to sell
        user_id = session.get("user_id")
        rows = db.execute("SELECT count FROM stocks WHERE stock_symbol = :symbol AND user_id = :user_id",
                symbol=symbol, user_id=user_id)
        if not rows:
            return apology("stock not owned", 400)
        elif len(rows) != 1 or rows[0]["count"] == None:
            return apology("something went wrong with the database", 500)

        owned_count = rows[0]["count"]
        if owned_count < share_count:
            return apology("too many shares", 400)
        # Sell the shares, if the count ges to 0, we delete the line in DB so that is does not appear in the index page anymore
        elif owned_count == share_count:
            db.execute("DELETE FROM stocks WHERE user_id = :user_id AND stock_symbol = :symbol",
                    user_id=user_id, symbol=symbol)
        else:
            new_count = owned_count - share_count
            db.execute("UPDATE stocks SET count = :new_count WHERE user_id = :user_id AND stock_symbol = :symbol",
                    new_count=new_count, symbol=symbol, user_id=user_id)

        # Credit the cash to the user
        rows = db.execute("SELECT cash FROM users WHERE id = :id", id=user_id)
        if len(rows) != 1 or not rows[0]["cash"]:
            return apology("something went wrong with the database", 500)

        transaction_credit = share_count * quote_data["price"]
        balance = rows[0]["cash"]
        new_balance = balance + transaction_credit

        # Make and register the transaction in DB
        db.execute("INSERT INTO transactions (user_id, symbol, share_count, price) " +
                "VALUES (:user_id, :symbol, :share_count, :price)",
                user_id=user_id, symbol=symbol, share_count=share_count*(-1), price=transaction_credit)
        db.execute("UPDATE users SET cash = :cash WHERE id = :id", cash=new_balance, id=user_id)

        # Flash this message so that it is displayed after redirection
        flash("Sold!")

        # Redirect user to home page
        return redirect("/")


@app.route("/moreCash", methods=["POST"])
@login_required
def get_more_cash():
    """User gets more cash"""

    user_id = session.get("user_id")
    cash_add = request.form.get("cash")

    # Various form controls
    if not cash_add:
        return apology("missing amount", 400)
    try:
        cash_float = float(cash_add)
    except (ValueError):
        return apology("amount of cash must be a number", 400)

    cash_cents = int(round(cash_float * 100))
    if cash_cents <= 0:
        return apology("do you not want to get richer ?", 400)
    elif cash_cents > 1000000:
        return apology("too greedy !", 400)

    # Go find user cash in DB
    rows = db.execute("SELECT cash FROM users WHERE id = :id", id=user_id)
    if len(rows) != 1 or not rows[0]["cash"]:
        return apology("something went wrong with the database", 500)

    # Increase his cash
    new_balance = rows[0]["cash"] + cash_cents / 100
    db.execute("UPDATE users SET cash = :cash WHERE id = :id", cash=new_balance, id=user_id)

    # Flash this message so that it is displayed after refresh
    flash("Got richer!")

    # Refresh home page
    return redirect("/")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
