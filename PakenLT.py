def parse(source,context):
    import re
    tokens = source.split("")
    AST = []
    for i in range(len(tokens)):
        cur_token = tokens[i]
        if re.match("[0-9]+",cur_token).end() == len(cur_token):
            node = Integer(context,int(cur_token))
            AST.append(node)
        if cur_token in ("+","-","*","/"):
            
            



class Context:
    count = 0	
    result = ""


class Node:
    def __init__(self,context):
        self.context = context
    def generate(self):
        raise NotImplementedError()


class Integer(Node):
    def __init__(self,context,num):
        super().__init__(context)
        self.num = num
    def generate(self):
        return str(self.num)


class BinaryExpression(Node):
    def __init__(self,context,left,right,op):
        super().__init__(context)
        self.left = left
        self.right = right
        self.op = op if op in ["+","-","*","/"] else ValueError()
    def generate(self):
        global count, result
        opcodes = {"+":"add","-":"sub","*":"mul","/":"div"}
        left = self.left.generate()
        right = self.right.generate()
        result += f"val{count} = {opcodes[self.op]} {left}, {right}\n"
        count += 1
        return "val"+str(count-1)


two = Integer(2)
three_plus_one = BinaryExpression(
    Integer(3),
    Integer(1),
    "+"
)

all_of_expression = BinaryExpression(two,three_plus_one,"*")
all_of_expression.generate()
print(result)