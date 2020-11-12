import h5py
import glob
import time

times = {}
files = glob.glob("DD0039/*.cpu*")[:2]
files.sort()
print(files)

def timeit(func):
    t1 = time.time()
    for N in range(100):
        func()
    t2 = time.time()
    times[func.__name__] = (t2-t1)/100

@timeit
def open_files():
    for f in files:
        f = h5py.File(f, "r")
        f.close()
print("Just to open:        %0.3e" % (times['open_files']))

@timeit
def open_top_level_group():
    for f in files:
        f = h5py.File(f, "r")
        g = f["/"]
        del g
        f.close()
print("To open group:       %0.3e" % (times['open_top_level_group']))

@timeit
def get_top_level_keys():
    for f in files:
        f = h5py.File(f, "r")
        g = f["/"].keys()
        del g
        f.close()
print("To open index:       %0.3e" % (times['get_top_level_keys']))

@timeit
def iter_top_level_groups():
    for f in files:
        f = h5py.File(f, "r")
        for g in f: pass
        f.close()
print("To iter items:       %0.3e" % (times['iter_top_level_groups']))

@timeit
def list_top_level_groups():
    for f in files:
        f = h5py.File(f, "r")
        list(f.items())
        f.close()
print("To list groups:      %0.3e" % (times['list_top_level_groups']))

@timeit
def recursive_iter():
    for f in files:
        f = h5py.File(f, "r")
        for n1, g in f.items():
            for n2, h in g.items():
                pass
        f.close()
print("To recurse groups:   %0.3e" % (times['recursive_iter']))

@timeit
def recursive_read():
    for f in files:
        f = h5py.File(f, "r")
        for n1, g in f.items():
            for n2, h in g.items():
                h[:]
        f.close()
print("To recurse and read: %0.3e" % (times['recursive_read']))
