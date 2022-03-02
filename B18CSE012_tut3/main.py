TYPE = 0
TIME = 1
TABLE_TYPE = 2
FROM_IP = 3
FROM_AS = 4
PREFIX = 5
AS_PATH = 6

def isInt(s):
    try: 
        int(s)
        return True
    except ValueError:
        return False


fileName = "bgp-routing-table.txt"
f = open(fileName, 'r')

uniqueAS = set()
ASDegree = dict()
uniquePREFIX = set()
instiA = list()
instiA_isp = set()
for line in f:
    words = line.split("|")
    #AS count
    as_path = words[AS_PATH].split()
    for i, AS in enumerate(as_path):
        if isInt(AS):
            uniqueAS.add(AS)

            #no of degrees
            if i==0 and len(as_path)>1:
                if AS in ASDegree:
                    ASDegree[AS].add(as_path[i])
                else:
                    ASDegree[AS] = set([as_path[i+1]])
            elif i==len(as_path)-1:
                if AS in ASDegree:
                    ASDegree[AS].add(as_path[i-1])
                else:
                    ASDegree[AS] = set([as_path[i-1]])
            else:
                if AS in ASDegree:
                    ASDegree[AS].add(as_path[i-1])
                    ASDegree[AS].add(as_path[i+1])
                else:
                    ASDegree[AS] = set([as_path[i-1], as_path[i+1]])

    
    #PREFIX count
    uniquePREFIX.add(words[PREFIX])

    #insti A(IITB) data
    ip = words[PREFIX].split("/")[0].split(".")
    if words[PREFIX].split("/")[1] == "24" and int(ip[0])==103 and int(ip[1])==21 and int(ip[2]) <= 127 and int(ip[2]) >= 124:
        instiA.append((words[PREFIX], words[AS_PATH]))

#insti A(IITB) isps
for ls in instiA:
    AS = ls[1].split()
    while AS[len(AS)-1] == '132423':
        AS.pop()
    instiA_isp.add(AS[len(AS)-1])

f1 = open("unique-as.txt", "w")
line = "No of unique AS: "+ str(len(uniqueAS)) + "\n"
f1.write(line)
print(line)
for AS in uniqueAS:
    f1.write(str(AS)+ "\n")
f1.close()

f1 = open("unique-prefix.txt", "w")
line = "No of unique PREFIX: "+ str(len(uniquePREFIX)) + "\n"
f1.write(line)
print(line)
for AS in uniquePREFIX:
    f1.write(str(AS)+ "\n")
f1.close()

f1 = open("instiA-data.txt", "w")
line = "No of calls to insti A: "+ str(len(instiA)) + "\n"
f1.write(line)
print(line)
for data in instiA:
    f1.write(str(data[0])+ "|"+ str(data[1]) + "\n")
f1.close()

f1 = open("instiA-isp.txt", "w")
line = "No of ISP provider for insti A: "+ str(len(instiA_isp)) + "\n"
f1.write(line)
print(line)
for data in instiA_isp:
    f1.write(str(data)+"\n")
f1.close()

f1 = open("top10-as.txt", "w")
print("Top 10 ASs:")
for AS, setAS in ASDegree.items():
    ASDegree[AS] = len(setAS)
for i, AS in enumerate( sorted(ASDegree, key=ASDegree.get, reverse=True)):
    line = str(AS)+ " Degrees:"+ str(ASDegree[AS]) + "\n"
    f1.write(line)
    print("\t"+line)
    if i>10:
        break
f1.close()