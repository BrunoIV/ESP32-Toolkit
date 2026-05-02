Import("env")
import os

input_dir = "images"
output_file = "data/templates/cmn/icons.svg"

with open(output_file, "w") as out:
    out.write('<svg xmlns="http://www.w3.org/2000/svg">\n')

    for filename in os.listdir(input_dir):
        if not filename.endswith(".svg"):
            continue

        filepath = os.path.join(input_dir, filename)

        name = os.path.splitext(filename)[0]

        out.write(f'<symbol id="{name}" viewBox="0 -960 960 960">\n')

        with open(filepath, "r") as f:
            content = f.read()

            content = content.replace("<path", "\n<path")

            content = content.replace("</svg", "\n")

            lines = content.splitlines()
            for line in lines:
                if "<path" in line:
                    out.write(line.strip() + "\n")

        out.write('</symbol>\n')

    out.write('</svg>\n')
