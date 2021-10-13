import os

# print(os.getcwd())
dir = os.getcwd()
meta_file_name = 'meta_data.txt'
file_name = 'test.txt'
output_folder = 'output'
SEP = '$'

folder_num = 0
output_folder += str(folder_num)


def find_in_meta(data, target):
    for i in data:
        if i[1] == target:
            return i
    return 'none'




#pull meta data
path = os.path.join(dir,meta_file_name)

pos = 0
last_pos = pos
file = open(path,"r")
conts = file.read()
meta_data= []
counter = 0
while(True):
    pos = conts.index(SEP,last_pos)
    
    meta_data.append((conts[last_pos:pos-1],conts[pos:pos+3]))
    # meta_data[counter][1] = conts[pos:pos+3]
    last_pos = pos+4
    if(last_pos == len(conts)):
        break




#create the path
path = os.path.join(dir,output_folder)
while(True):
    if(os.path.isdir(path)):
        folder_num+=1
        output_folder = output_folder[:6]+ str(folder_num)
        path = os.path.join(dir,output_folder)
    else:
        os.mkdir(path)
        break
output_folder = path


#create files in output path
for i in meta_data:
    path = os.path.join(output_folder, i[0])
    try:
        file = open(path, "x")
    except:
        print("File exisits")
    finally:
        file.close()



buffer = ''

print(conts)
done = False
length = 4000
path = os.path.join(dir,file_name)
data_file = open(path, "r")
while(not done):
    
    pos = 0
    last_pos = pos
    location = ''
    conts = data_file.read(length)
    if(len(conts) < length):
        done = True
    conts = buffer + conts
    while(True):
        try:
            pos = conts.index(SEP,last_pos)
            location = conts[pos:pos+3]
            print(location)
            if(not find_in_meta(meta_data,location) == 'none'):
                # print(find_in_meta(meta_data,location)[0])
                path = os.path.join(output_folder, find_in_meta(meta_data,location)[0])
                file = open(path,"a")
                file.write(conts[last_pos:pos])
                file.close()
                last_pos = pos+3
            else:
                buffer = conts[last_pos:len(conts)]
                break
            
        except ValueError:
            buffer = conts[last_pos:len(conts)]
            break
data_file.close()


