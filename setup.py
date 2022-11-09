import glob, os

files = glob.glob("**/.project.templated", recursive=True)
files2 = (glob.glob("**/.cproject.templated", recursive=True))

allFiles = files + files2

print(allFiles)
for file in allFiles :
    with open(file, "rt") as fin:
        with open(file.replace(".templated",""), "wt") as fout:
            for line in fin:

                line = line.replace('${ROOT_DIR}', os.getcwd().replace(os.sep, '/') + "/")
                line = line.replace("\\","/")
                    
                fout.write(line)


