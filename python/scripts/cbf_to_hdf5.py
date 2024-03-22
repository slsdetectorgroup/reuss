from pathlib import Path
import argparse
import hdf5plugin
import h5py
import cbf
import numpy as np

def string_dt(s):
    tid = h5py.h5t.C_S1.copy()
    tid.set_size(len(s))
    return h5py.Datatype(tid)

def create_string_attr(obj, key, value):
    #Albula requires null terminated strings
    if value[-1] != "\x00":
        value += "\x00"
    obj.attrs.create(key, value, dtype=string_dt(value))




def convert_cbf_to_hdf5(path_in, file_out, pixel_mask = None):

    #Create output directory if it does not exist
    path_out = file_out.parent
    path_out.mkdir(exist_ok=True)

    #Find all cbf files
    files = list(path_in.glob("*.cbf"))
    files.sort()


    #Read the first file to find out shape and dtype
    data = cbf.read(files[0]).data
    dt = data.dtype
    shape = (len(files), *data.shape)

    #Create pixel mask if it does not exist
    if pixel_mask is None:
        pixel_mask = np.zeros(shape, dtype = np.uint8) 

    print(f"Writing to: {file_out}")
    with h5py.File(file_out, "w") as f:
        compression=hdf5plugin.Bitshuffle(nelems=0, cname='lz4')
        nxentry = f.create_group("entry")
        create_string_attr(nxentry, "NX_class", "NXentry")
        nxdata = nxentry.create_group("data")
        create_string_attr(nxdata, "NX_class", "NXdata")
        ds = nxdata.create_dataset(
            "data_000001",
            shape=shape,
            dtype=dt,
            maxshape=(None, *shape[1:]),
            chunks = (1, *shape[1:]),
            **compression,
        )

        for i, file in enumerate(files):
            ds[i] = cbf.read(file).data
            print(f"{file.name}", end = '\r')
        print()
        ds.attrs["image_nr_low"] = np.int32(0)
        ds.attrs["image_nr_high"] = np.int32(shape[0] - 1)

        #Pixel mask for albula
        inst = nxentry.create_group("instrument/detector/detectorSpecific")
        inst.create_dataset("pixel_mask", data=pixel_mask.astype(np.uint8), **compression)

if __name__ == '__main__':
    path_in = Path("/home/l_frojdh/data/029_21-Feb-2024_153852/cbf_200")
    parser = argparse.ArgumentParser()
    parser.add_argument("path_in", help="Path to the input directory", type=Path)
    parser.add_argument("file_out", help="Path and filename of the output file", type=Path)
    args = parser.parse_args()
    convert_cbf_to_hdf5(args.path_in, args.file_out)