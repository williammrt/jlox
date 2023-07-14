import sys

def define_type(h_writer, cpp_writer, base_name, class_name, field_list):
    # .h
    h_writer.write(f"class {base_name}::{class_name} final: public {base_name} {{\n")
    h_writer.write("public:\n")
    # member
    fields = field_list.split(", ")
    for field in fields:
        h_writer.write(f"\t{field};\n")
    # constructor
    h_writer.write("\n")
    h_writer.write(f"\t{class_name}({field_list});\n")
    # accept
    h_writer.write(f"\tObject accept(Visitor* visitor) override;\n")
    h_writer.write("};\n\n")

    # .cpp
    # constructor
    cpp_writer.write(f"{base_name}::{class_name}::{class_name}({field_list}): ")
    init_list = ""
    for field in fields:
        arg_type, name = field.split(" ")
        print(arg_type)
        if arg_type == 'Token':
            init_list += f"{name} {{{name}}}, "
        else:
            init_list += f"{name} {{std::move({name})}}, "
    init_list = init_list[:-2]
    cpp_writer.write(init_list)
    cpp_writer.write(" {}\n\n")
    # accept
    cpp_writer.write(f"Object {base_name}::{class_name}::accept(Visitor* visitor) {{\n")
    cpp_writer.write(f"\treturn visitor->visit_{class_name}_{base_name}(this);\n")
    cpp_writer.write("};\n\n")
    
def define_ast(output_dir, base_name, types):
    path = output_dir + "/" + base_name
    h_writer = open(path+".h", "w+")
    cpp_writer = open(path+".cpp", "w+")
    
    # .h include guard
    h_writer.write(f"#ifndef {base_name}_H\n")
    h_writer.write(f"#define {base_name}_H\n\n")

    # .h header
    h_writer.write('#include <vector>\n')
    h_writer.write('#include <memory>\n')
    h_writer.write('#include "Token.h"\n')
    h_writer.write('#include "Object.h"\n')
    if base_name != 'Expr':
        h_writer.write('#include "Expr.h"\n\n')


    # .h ABC
    h_writer.write(f"class {base_name} {{\n")
    h_writer.write("public:\n")
    # .h forward declaration
    for t in types:
        # add space in split to allow std::
        class_name = t.split(": ")[0].strip()
        h_writer.write(f"\tclass {class_name};\n")
    h_writer.write("\n")

    h_writer.write(f"\tclass Visitor;\n\n")
    
    h_writer.write(f"\tvirtual Object accept(Visitor* visitor) = 0;\n")
    h_writer.write("};\n\n")

    # .h Visitor
    h_writer.write(f"class {base_name}::Visitor {{\n")
    h_writer.write("public:\n")
    for t in types:
        class_name = t.split(": ")[0].strip()
        h_writer.write(f"\tvirtual Object visit_{class_name}_{base_name}({class_name}* expr)=0;\n")
    h_writer.write(f"}};\n\n")

    # .cpp header
    cpp_writer.write(f"#include \"{base_name}.h\"\n\n")

    # subclass
    for t in types:
        class_name = t.split(": ")[0].strip()
        fields = t.split(": ")[1].strip()
        define_type(h_writer, cpp_writer, base_name, class_name, fields)
    
    # .h include guard
    h_writer.write("#endif\n")

if (len(sys.argv) != 2):
    print("Usage: generate_ast.py <output directory>")
    exit(64)
    
output_dir = sys.argv[1]
define_ast(output_dir, "Expr", [
    "Assign   : Token name, std::unique_ptr<Expr> value",
    "Binary   : std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right",
    "Call     : std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments",
    "Grouping : std::unique_ptr<Expr> expression",
    "Literal  : Object value",
    "Logical  : std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right",
    "Unary    : Token op, std::unique_ptr<Expr> right",
    "Variable : Token name"
    ])

define_ast(output_dir, "Stmt", [
    "Block      : std::vector<std::unique_ptr<Stmt>> statements",
    "Expression : std::unique_ptr<Expr> expression",
    "Function   : Token name, std::vector<Token> params," +
                  " std::vector<std::unique_ptr<Stmt>> body",
    "If         : std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch," +
                  " std::unique_ptr<Stmt> elseBranch",
    "Print      : std::unique_ptr<Expr> expression",
    "Return     : Token keyword, std::unique_ptr<Expr> value",
    "Var        : Token name, std::unique_ptr<Expr> initializer",
    "While      : std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body"
    ])


