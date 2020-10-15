with open('app/x64/Debug/app.elf', 'rb') as file:
    data = file.read()

q = int.from_bytes(data[24:32], 'little')
q -= 0x80000000
data = data[:24] + q.to_bytes(8, 'little') + data[32:]

with open('app/x64/Debug/app.elf', 'wb') as file:
    file.write(data)
