from obj_loader import load_obj
from PIL import Image
import struct
import sys

vertices, indices, faces_by_mtl = load_obj(sys.argv[1])

with open(sys.argv[2], "wb") as dst:
    dst.write(len(vertices).to_bytes(4, byteorder="little"))
    dst.write(vertices.tobytes())

    dst.write(len(indices).to_bytes(4, byteorder="little"))
    dst.write(indices.tobytes())

    dst.write(len(faces_by_mtl).to_bytes(4, byteorder="little"))
    for material, start_index, end_index in faces_by_mtl:
        dst.write(start_index.to_bytes(4, byteorder="little"))
        dst.write(end_index.to_bytes(4, byteorder="little"))

        for comp in ("Ka", "Kd", "Ks"):
            image = material.get("map_" + comp, None)
            if image:
                image = image.convert("RGBA").transpose(Image.FLIP_TOP_BOTTOM)
                dst.write(struct.pack("<2I", *image.size))
                dst.write(image.tobytes())

            else:
                color_bytes = bytearray([0, 0, 0, 255])
                color = material.get(comp, None)
                if color is not None:
                    for i, x in enumerate(color):
                        color_bytes[i] = int(x * 255)

                dst.write(b"\x01\x00\x00\x00\x01\x00\x00\x00")
                dst.write(color_bytes)

        image = material.get("map_Ns", None)
        if image:
            raise NotImplementedError("check me out")

        else:
            specular_power = material.get("Ns", 0.0)
            dst.write(b"\x01\x00\x00\x00\x01\x00\x00\x00" + struct.pack("<f", specular_power))
