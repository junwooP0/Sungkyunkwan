import sys
import struct

def read_file(filename):
    with open(filename, 'rb') as file:
        content = file.read()
    return content

def cover_sign(value, bits):
    sign = 1 << (bits - 1)
    if (value & sign):
        value -= (1 << bits)
    return value


def to_signed(value):
    return cover_sign(value, 32)


def decode_binary(instruction):
    opcode = instruction & 0x7f
    rd = (instruction >> 7) & 0x1f
    funct3 = (instruction >> 12) & 0x7
    rs1 = (instruction >> 15) & 0x1f
    rs2 = (instruction >> 20) & 0x1f
    funct7 = (instruction >> 25) & 0x7f
    imm = 0
    temp = 0
    # R-type
    if opcode == 0x33:
        if funct7 == 0x00:
            if funct3 == 0x00:
                return f"add x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x01:
                return f"sll x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x2:
                return f"slt x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x3:
                return f"sltu x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x4:
                return f"xor x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x5:
                return f"srl x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x6:
                return f"or x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x7:
                return f"and x{rd}, x{rs1}, x{rs2}"
        elif funct7 == 0x20:
            if funct3 == 0x00:
                return f"sub x{rd}, x{rs1}, x{rs2}"
            elif funct3 == 0x05:
                return f"sra x{rd}, x{rs1}, x{rs2}"
            
        
    # I-type
    elif opcode == 0x37:
        imm = instruction & 0xfffff000
        if imm & 0x80000000:
            imm -= 0x100000000
        return f"lui x{rd}, {imm}"
    

    # I-type
    elif opcode == 0x17:
        imm = instruction & 0xfffff000
        return f"auipc x{rd}, {imm}"
    
    # S-type
    elif opcode == 0x6f:
        temp1 = (instruction >> 12) & 0xff
        temp2 = (instruction >> 20) & 1
        temp3 = (instruction >> 21) & 0x3ff
        temp4 = (instruction >> 31) & 1
        temp = temp1 << 12 | temp2 << 11 | temp3 << 1 | temp4 << 20
        imm = cover_sign(temp, 21)
        return f"jal x{rd}, {imm}"
   # I-type         
    elif opcode == 0x67:
        temp = instruction >> 20
        imm = cover_sign(temp, 12)
        return f"jalr x{rd}, {imm}(x{rs1})"       
    # I-type
    elif opcode == 0x13:
        temp = instruction >> 20
        imm = cover_sign(temp, 12)
        if funct3 == 0x00:
            return f"addi x{rd}, x{rs1}, {imm}"
        elif funct3 == 0x2:
            return f"slti x{rd}, x{rs1}, {imm}"
        elif funct3 == 0x3:
            return f"sltiu x{rd}, x{rs1}, {imm}"
        elif funct3 == 0x4:
            return f"xori x{rd}, x{rs1}, {imm}"
        elif funct3 == 0x6:
            return f"ori x{rd}, x{rs1}, {imm}"
        elif funct3 == 0x7:
            return f"andi x{rd}, x{rs1}, {imm}"
        elif funct3 == 0x01:
            shamt = rs2
            return f"slli x{rd}, x{rs1}, {shamt}"
        # I-type
        elif funct3 == 0x05:
            shamt = rs2
            if (instruction >> 30) == 0:
                return f"srli x{rd}, x{rs1}, {shamt}"
            elif (instruction >> 30) == 1:
                return f"srai x{rd}, x{rs1}, {shamt}"
        
    # S-type    
    elif opcode == 0x63:
        temp1 = (instruction >> 8) & 0xf 
        temp2 = (instruction >> 25) & 0x3f 
        temp3 = (instruction >> 7) & 0x1  
        temp4 = (instruction >> 31) & 0x1 
    
        temp = temp4 << 12 | temp2 << 5 | temp1 << 1 | temp3 << 11 
        imm = cover_sign(temp, 13)
        if funct3 == 0x0:
            return f"beq x{rs1}, x{rs2}, {imm}"
        elif funct3 == 0x1:
            return f"bne x{rs1}, x{rs2}, {imm}"
        elif funct3 == 0x4:
            return f"blt x{rs1}, x{rs2}, {imm}"
        elif funct3 == 0x5:
            return f"bge x{rs1}, x{rs2}, {imm}"
        elif funct3 == 0x6:
            return f"bltu x{rs1}, x{rs2}, {imm}"
        elif funct3 == 0x7:
            return f"bgeu x{rs1}, x{rs2}, {imm}"
    
    # I-type    
    elif opcode == 0x03:
        imm = cover_sign(instruction >> 20, 12)
        if funct3 == 0x0:
            return f"lb x{rd}, {imm}(x{rs1})"
        elif funct3 == 0x1:
            return f"lh x{rd}, {imm}(x{rs1})"
        elif funct3 == 0x2:
            return f"lw x{rd}, {imm}(x{rs1})"
        elif funct3 == 0x4:
            return f"lbu x{rd}, {imm}(x{rs1})"
        elif funct3 == 0x5:
            return f"lhu x{rd}, {imm}(x{rs1})"
    # S-type     
    elif opcode == 0x23:
        temp1 = (instruction >> 25) & 0x7f
        temp2 = (instruction >> 7) & 0x1f
        temp = (temp1 << 5) | temp2
        imm = cover_sign(temp, 12)
        if funct3 == 0x0:
            return f"sb x{rs2}, {imm}(x{rs1})"
        elif funct3 == 0x1:
            return f"sh x{rs2}, {imm}(x{rs1})"
        elif funct3 == 0x2:
            return f"sw x{rs2}, {imm}(x{rs1})"
        
    
    return "unknown instruction"

def disassemble_binary(data):
    instructions = []
    for i in range(0, len(data), 4):
        whole = data[i:i+4]
        byte0 = whole[0]
        byte1 = whole[1]
        byte2 = whole[2]
        byte3 = whole[3]
        instruction = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0
        
        disassembled = decode_binary(instruction)
        
        index = i // 4
        hexadecimal = format(instruction, '08x')
        output = "inst " + str(index) + ": " + hexadecimal + " " + disassembled
        instructions += [output]

    return instructions

def init_register():
    return [0] * 32

def execute_instruction(instruction, registers, memory, pc):
    opcode = instruction & 0x7f
    rd = (instruction >> 7) & 0x1f
    funct3 = (instruction >> 12) & 0x7
    rs1 = (instruction >> 15) & 0x1f
    rs2 = (instruction >> 20) & 0x1f
    funct7 = (instruction >> 25) & 0x7f
    imm = 0
    next_pc = pc + 4 

    if opcode == 0x33:  # R-type
        if funct7 == 0x00:
            if funct3 == 0x00:  # add
                registers[rd] = registers[rs1] + registers[rs2]
            elif funct3 == 0x01:  # sll
                registers[rd] = registers[rs1] << (registers[rs2] & 0x1f)
            elif funct3 == 0x2:  # slt
                registers[rd] = int(to_signed(registers[rs1]) < to_signed(registers[rs2]))
            elif funct3 == 0x3:  # sltu
                registers[rd] = int((registers[rs1] & 0xffffffff) < (registers[rs2] & 0xffffffff))
            elif funct3 == 0x4:  # xor
                registers[rd] = registers[rs1] ^ registers[rs2]
            elif funct3 == 0x5:  # srl
                registers[rd] = (registers[rs1] & 0xffffffff) >> (registers[rs2] & 0x1f)
            elif funct3 == 0x6:  # or
                registers[rd] = registers[rs1] | registers[rs2]
            elif funct3 == 0x7:  # and
                registers[rd] = registers[rs1] & registers[rs2]
        elif funct7 == 0x20:
            if funct3 == 0x00:  # sub
                registers[rd] = registers[rs1] - registers[rs2]
            elif funct3 == 0x05:  # sra
                shift_amount = registers[rs2] & 0x1f
                value = registers[rs1]
                if value & 0x80000000:
                    registers[rd] = (value >> shift_amount) | (0xffffffff << (32 - shift_amount))
                else:
                    registers[rd] = value >> shift_amount

    elif opcode == 0x13:  # I-type
        imm = cover_sign(instruction >> 20, 12)
        if funct3 == 0x00:  # addi
            registers[rd] = registers[rs1] + imm
        elif funct3 == 0x2:  # slti
            registers[rd] = int(to_signed(registers[rs1]) < imm)
        elif funct3 == 0x3:  # sltiu
            registers[rd] = int((registers[rs1] & 0xffffffff) < (imm & 0xffffffff))
        elif funct3 == 0x4:  # xori
            registers[rd] = registers[rs1] ^ imm
        elif funct3 == 0x6:  # ori
            registers[rd] = registers[rs1] | imm
        elif funct3 == 0x7:  # andi
            registers[rd] = registers[rs1] & imm
        elif funct3 == 0x01:  # slli
            shamt = rs2
            registers[rd] = registers[rs1] << shamt
        elif funct3 == 0x05:
            shamt = rs2
            if (instruction >> 30) == 0:  # srli
                registers[rd] = (registers[rs1] & 0xffffffff) >> shamt
            elif (instruction >> 30) == 1:  # srai
                shift_amount = shamt
                value = registers[rs1]
                if value & 0x80000000:
                    registers[rd] = (value >> shift_amount) | (0xffffffff << (32 - shift_amount))
                else:
                    registers[rd] = value >> shift_amount

    elif opcode == 0x37:  # U-type
        imm = instruction & 0xfffff000
        registers[rd] = imm

    elif opcode == 0x17:  # U-type
        imm = instruction & 0xfffff000
        registers[rd] = imm + (pc & 0xfff)

    elif opcode == 0x6f:  # J-type
        temp = ((instruction >> 12) & 0xff) << 12 | ((instruction >> 20) & 1) << 11 | ((instruction >> 21) & 0x3ff) << 1 | (instruction >> 31) << 20
        imm = cover_sign(temp, 21)
        registers[rd] = pc + 4
        next_pc = pc + imm

    elif opcode == 0x67:  # I-type
        imm = cover_sign(instruction >> 20, 12)
        registers[rd] = pc + 4
        next_pc = (registers[rs1] + imm) & ~1

    elif opcode == 0x63:  # SB-type
        imm = cover_sign(((instruction >> 8) & 0xf) << 1 | ((instruction >> 25) & 0x3f) << 5 | ((instruction >> 7) & 0x1) << 11 | (instruction >> 31) << 12, 13)
        if funct3 == 0x0:  # beq
            if registers[rs1] == registers[rs2]:
                next_pc = pc + imm
        elif funct3 == 0x1:  # bne
            if registers[rs1] != registers[rs2]:
                next_pc = pc + imm

    elif opcode == 0x03:  # I-type
        imm = cover_sign(instruction >> 20, 12)
        address = (registers[rs1] + imm) // 4 
        if registers[rs1] + imm == 0x20000000:
            user_input = int(input())
            registers[rd] = user_input
        else:
            if funct3 == 0x0:  # lb
                registers[rd] = memory[address] & 0xff
                if registers[rd] & 0x80: 
                    registers[rd] |= 0xffffff00
            elif funct3 == 0x1:  # lh
                registers[rd] = memory[address] & 0xffff
                if registers[rd] & 0x8000: 
                    registers[rd] |= 0xffff0000
            elif funct3 == 0x2:  # lw
                registers[rd] = memory[address]
            elif funct3 == 0x4:  # lbu
                registers[rd] = memory[address] & 0xff
            elif funct3 == 0x5:  # lhu
                registers[rd] = memory[address] & 0xffff

    elif opcode == 0x23:  # S-type
        imm = cover_sign(((instruction >> 25) & 0x7f) << 5 | ((instruction >> 7) & 0x1f), 12)
        address = (registers[rs1] + imm) // 4 
        if registers[rs1] + imm == 0x20000000:
            print(chr(registers[rs2] & 0xff), end='')
        else:
            if funct3 == 0x0:  # sb
                memory[address] = registers[rs2] & 0xff
            elif funct3 == 0x1:  # sh
                memory[address] = registers[rs2] & 0xffff
            elif funct3 == 0x2:  # sw
                memory[address] = registers[rs2]

    return next_pc

def print_registers(registers):
    for i in range(32):
        print(f"x{i}: 0x{registers[i] & 0xffffffff:08x}")

def load_memory(data_file, memory):
    data = read_file(data_file)
    for i in range(0, len(data), 4):
        word = struct.unpack_from('<I', data, i)[0]
        address = 0x10000000 + i
        memory[address // 4] = word 

def main():

    binary = sys.argv[1]
    num_instructions = int(sys.argv[-1])
    data_file = None

    if len(sys.argv) == 4:
        data_file = sys.argv[2]

    data = read_file(binary)
    registers = init_register()
    memory = [0] * (0x10000000 // 4 + 20000)
    pc = 0

    if data_file:
        load_memory(data_file, memory)

    executed_instructions = 0

    while executed_instructions < num_instructions and pc < len(data):
        instruction = struct.unpack_from('<I', data, pc)[0]
        pc = execute_instruction(instruction, registers, memory, pc)
        registers[0] = 0 
        executed_instructions += 1


    print_registers(registers)

if __name__ == "__main__":
    main()