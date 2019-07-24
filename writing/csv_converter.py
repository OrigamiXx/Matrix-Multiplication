import csv

def print_data(data, k):
    print("======= k = %d =====" % curr_k)
    ss = list(data.keys())
    ss.sort()
    for i in range(len(field_idxs)):
        print(fields[i])
        for s in ss:
            if data[s][i] == -1:
                continue
            print("(" + str(s) + "," + str(data[s][i]) + ")")
        print()
    print("----- %USP -----")
    for s in ss:
        print("(" + str(s) + "," + str(data[s][-1]) + ")")
    print()
    

reader = csv.reader(open("paper_data01.csv"))


rows = []
for row in reader:
    rows.append(row)

header = rows[0]
rows.remove(header)
    
s_idx = header.index("s")
k_idx = header.index("k")
usp_idx = header.index("Full-Total Count")
is_usp_idx = header.index("Full-IS_USP Count")

fields = ["UNI-Total Mean (sec)",
          "BI-Total Mean (sec)",
          "SAT-Total Mean (sec)",
          "MIP-Total Mean (sec)",
          "Full-Total Mean (sec)"]

field_idxs = []
for field in fields:
    field_idxs.append(header.index(field))

curr_k = int(rows[0][k_idx])
k = curr_k
data = {}
for row in rows:
    k = int(row[k_idx])
    if curr_k != k:
        print_data(data, k)
        data = {}
        curr_k = k
        
    s = int(row[s_idx])
    usp = int(row[usp_idx])
    is_usp = int((0 if row[is_usp_idx] == '' else row[is_usp_idx]))

    data[s] = []
    for field_idx in field_idxs:
        data[s].append(row[field_idx] if row[field_idx] != '' else -1)
    percent = is_usp / usp * 100.0
    data[s].append(percent)
        
    #print(data)
    #print()
    #print(row)

print_data(data, k)
