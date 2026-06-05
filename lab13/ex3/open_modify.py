from bcc import BPF
import time 

try:
    b = BPF(src_file="open_modify.bpf.c")
    b.trace_print()

except KeyboardInterrupt:
    exit()