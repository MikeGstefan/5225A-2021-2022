path = 'E:\test.txt'

file = open(path,"r")
conts = file.read()

done = False
pos = 0
last_pos = pos
while(not done):
    pos = conts.find("#",last_pos)


