
import re
import os
import sys
from clang.cindex import *
from collections import defaultdict
import uuid
import json


# if debug:
#     print("%s %s %s %s %s" % (t.kind, t.cursor.spelling, t.cursor.kind, t.spelling, str(t.location.column)))
# if t.kind in [TokenKind.IDENTIFIER]  and t.cursor.kind in [CursorKind.PARM_DECL, CursorKind.FIELD_DECL,CursorKind.VAR_DECL,CursorKind.CLASS_DECL,CursorKind.FUNCTION_DECL,CursorKind.CXX_METHOD,CursorKind.NAMESPACE]:
    #     out = []
    #     GetWordPattern(t.spelling, out, debug)
    #     result[t.spelling]=out

def GetAllTokenDetail(cursor):
    result = {"include":set(), "class":set(), "define":set(), "dependent_class":set()}
    for t in cursor.get_tokens():
        if t.cursor.kind in [CursorKind.CLASS_DECL]:
            result["class"].add(t.cursor.displayname)
        if t.cursor.kind in [CursorKind.MACRO_DEFINITION]:
            result["define"].add(t.cursor.displayname)
        if t.cursor.kind in [CursorKind.INCLUSION_DIRECTIVE]:
            result["include"].add(t.cursor.displayname)
        if t.cursor.kind in [CursorKind.TYPE_REF]:
            class_name = t.cursor.displayname
            if ' ' not in class_name:
                result["dependent_class"].add(t.cursor.displayname)
    return result

        


# Config.set_library_path("/home/jiacheng/work/llvm-5.0.1/lib")
Config.set_library_path("/hx/install/llvm/lib")

index = Index.create()

# file = "/home/jiacheng/work/clang/hpc_db/writer.h"

for i in range(1, len(sys.argv)):

    data_path = sys.argv[i]

    tmp = str(uuid.uuid4())

    os.system('find ./{} -type f -regex ".*\.[h|cpp]+" > {}'.format(data_path, tmp))

    files = open(tmp).read().split("\n")

    os.system('rm {}'.format(tmp))

    result = {"data":[]}


    for file in files:
        # print(file)
        if file.endswith(".h") or file.endswith(".cpp"):
            tu = index.parse(file, ['-std=c++11','-std=c++14'], options=TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
            st = GetAllTokenDetail(tu.cursor)
            st["module"] = data_path
            st["file"] = file

            st["include"] = list(st["include"])
            st["class"] = list(st["class"])
            st["define"] = list(st["define"])
            st["dependent_class"] = list(st["dependent_class"])

            print(file)
            print(st)
            result["data"].append(st)

            # if len(st["dependent_class"]) != 0:
            #     print(st["dependent_class"])
            print("-"*20)
    output = open("{}_output".format(data_path), "w")
    json.dump(result, output)

