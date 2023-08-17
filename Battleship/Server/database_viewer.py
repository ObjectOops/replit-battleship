from replit import db
import os

while True:
    c = input("Key: ")
    if c == 'q':
        break
    elif c == "boots":
        for boot in db[c]:
            print(boot)
    elif c == "connection_log":
        for connection in db[c]:
            print(connection[0], connection[1], connection[2])
    elif c == "like_log":
        for like in db[c]:
            print(like[0], like[1])
    elif c == "game_log":
        for game in db[c]:
            print('[' + game[0] + ']', '[' + game[1] + ']', game[2])
    elif c == "games":
        for game in db[c]:
            print(game)
            for thing in db[c][game]:
                print('\t' + str(thing))
    elif c == "likes":
        print(db[c])
    else:
        print("Invalid key.")