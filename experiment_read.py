import sys
import shutil
import subprocess
import os
import csv
import glob
import matplotlib.pyplot as plt
import numpy as np

def read_fixed_len_pages(pageFile, pageSize):
    """
    Execute ./read_fixed_len_page and return how long it took to read the file.
    """
    result = subprocess.run(
        [
            os.path.join(os.path.dirname(os.path.realpath(__file__)),'./read_fixed_len_pages'),
            pageFile,
            str(pageSize),
            "no-stdout",
        ],
        stdout=subprocess.PIPE,
    )
    # print (bytes.decode(result.stdout).split('\n'))
    return bytes.decode(result.stdout).split('\n')[2].split(' ')[1]


def main():
    
    if len(sys.argv) != 2:
        print('Usage: experiment_read <pageFile>')
        sys.exit(1)
    pageFile = sys.argv[1]
    totalSize = 2e7
    
    # pageSizes = [
    # 1000,
    # 2000,
    # 4000,
    # 8000,
    # 16000,
    # 32000,
    # 64000,
    # 128000   
    # ]
    pageSizes = [
        1 * 2 ** 10,   # 1 KB
        2 * 2 ** 10,   # 2 KB
        4 * 2 ** 10,   # 4 KB
        8 * 2 ** 10,   # 8 KB
        16 * 2 ** 10,   # 16 KB
        32 * 2 ** 10,   # 32 KB
        64 * 2 ** 10,   # 64 KB
        128 * 2 ** 10,   # 64 KB
        256 * 2 ** 10,  # 256 KB
        512 * 2 ** 10,  # 512 KB
        1 * 2 ** 20,  # 1 MB
        2 * 2 ** 20,  # 2 MB
        4 * 2 ** 20,  # 4 MB
        8 * 2 ** 20,  # 8 MB
        16 * 2 ** 20  # 16 MB
    ]
    sizeList = []    
    timeTookList = []
    aveRateList = []
    timeAve = 0

    csvwriter = csv.DictWriter(sys.stdout, fieldnames=('page_size', 'time_took'))
    csvwriter.writeheader()
    for size in pageSizes:
        aveRate = 0
        timeAve = 0
        timeTookList[:] = []
        for i in range(3):
            timeTook = read_fixed_len_pages(
                pageFile,
                size,
            )
            csvwriter.writerow({
                'page_size': size,
                'time_took': timeTook,
            })
            
            timeTookList.append(int(timeTook))
        
        timeAve = sum(timeTookList)/3
        aveRate = totalSize/timeAve
        sizeList.append(int(size))
        aveRateList.append(aveRate)

    plt.scatter(sizeList, aveRateList)
    plt.plot(sizeList, aveRateList)
    plt.xlabel("Page Size (Bytes)")
    plt.ylabel("Read Data Rates (records/s)")
    plt.xscale('log', basex=2)

    plt.ylim(0, 100000)
    # plt.ylim(0, 8e6)  for usb write
    plt.title('Read ::: ',fontweight='bold')
    plt.show()

if __name__ == '__main__':
    main()