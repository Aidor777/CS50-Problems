function processRating()
{
    let rating = parseInt(document.querySelector("#rating").value, 10);

    if(isNaN(rating))
    {
        alert("Woops, something went wrong !");
    }
    else if(rating === 50)
    {
        alert("Are you a bot ?");
    }
    else if(rating <= 20)
    {
        alert("Are you insane ? Or are you just trying to anger me ?");
    }
    else if(rating < 50)
    {
        alert("I strongly disagree with you, but hey that's life. We can't be friends tough.");
    }
    else if(rating < 80)
    {
        alert("Come on ! This country is so nice ! You should really give it another try !");
    }
    else
    {
        alert("Yeah I know it's awesome, and I can't wait to go back there ! Meet you there next time :)");
    }

    return false;
}