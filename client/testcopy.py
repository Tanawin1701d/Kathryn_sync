import shutil
file = open("result.csv","r")
files = open("result3.csv", "w")


shutil.copyfileobj(file, files)

file.close()
files.close()