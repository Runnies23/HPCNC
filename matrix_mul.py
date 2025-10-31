matrix1 = [
    [1,2,3],
    [4,5,6],
    [7,8,9]
]

matrix2 = [
    [1,2,3],
    [4,5,6],
    [7,8,9]
]


rows = len(matrix1)
cols = len(matrix1[0])

new_matrix = []

assert len(matrix1) == len(matrix2)

for row in range(rows):

    temp_lst = []
    for col in range(cols):
        
        temp = 0
        for i in range(rows):
            temp += matrix1[row][i] * matrix2[i][col]
            
        # print(row, col, temp)
        temp_lst.append(temp)
    new_matrix.append(temp_lst)

# print(new_matrix)

for i in (new_matrix):
    for j in i:
        print(j, end="\t")

    print()