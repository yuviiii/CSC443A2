import sys
import shutil
import subprocess
import os
import csv
import glob
import matplotlib.pyplot as plt
import numpy as np

def write_fixed_len_pages(csvFile, outputFile, pageSize):
    """
    Execute ./write_fixed_len_pages and return how long it took to write the file.
    """
    result = subprocess.run(
        [
            os.path.join(os.path.dirname(os.path.realpath(__file__)), './write_fixed_len_pages'),
            csvFile,
            outputFile,
            str(pageSize),
        ],
        stdout=subprocess.PIPE,
    )
    return bytes.decode(result.stdout).split('\n')[2].split(' ')[1]


def main():

    if len(sys.argv) != 3:
        print('Usage: experiment_write.py <csvFile> <outputFile>')
        sys.exit(1)
    csvFile = sys.argv[1]
    
    outputFile = sys.argv[2]
    totalSize = 2e7
    pageSizes = [
        1 * 2 ** 10,   # 1 KB
        2 * 2 ** 10,   # 2 KB
        4 * 2 ** 10,   # 4 KB
        8 * 2 ** 10,   # 8 KB
        16 * 2 ** 10,   # 16 KB
        32 * 2 ** 10,   # 32 KB
        64 * 2 ** 10,   # 64 KB
        128 * 2 ** 10,  # 128 KB
        256 * 2 ** 10,  # 256 KB
        512 * 2 ** 10,  # 512 KB
        1 * 2 ** 20,  # 1 MB
        2 * 2 ** 20,  # 2 MB
        4 * 2 ** 20,  # 4 MB
        8 * 2 ** 20,  # 8 MB
        16 * 2 ** 20  # 16 MB
    ]
    
    # pageSizes = [
    # 1000,
    # 2000,
    # 4000,
    # 8000,
    # 16000,
    # 32000,
    # 64000,
    # 128000,
    # 256000,
    # 512000,
    # 1024000,
    # 2048000,
    # 4096000
    # ]

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

            timeTook = write_fixed_len_pages(
                csvFile,
                outputFile,
                size,
            )
            csvwriter.writerow({
                'page_size': size,
                'time_took': int(timeTook),
            })

            timeTookList.append(int(timeTook))
        
        timeAve = sum(timeTookList)/3
        aveRate = totalSize/timeAve
        sizeList.append(int(size))
        aveRateList.append(aveRate)
    print(aveRateList)
    plt.scatter(sizeList, aveRateList)
    plt.plot(sizeList, aveRateList)
    plt.xlabel("Page Size (Bytes)")
    plt.ylabel("Write Data Rates (records/s)")
    plt.xscale('log', basex=2)

    plt.ylim(0, max(aveRateList)+5000)
    # plt.ylim(0, 8e6)  for usb write
    plt.title('Write ::: ',fontweight='bold')
    plt.show()

if __name__ == '__main__':
    main()