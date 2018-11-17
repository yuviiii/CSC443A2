import sys
import shutil
import subprocess
import os
import csv
import glob
import matplotlib.pyplot as plt
import numpy as np

def select(pageFile, attributeId, start, end, pageSize):
    """
    Execute ./select and return how long it took to process
    """
    result = subprocess.run(
        [
            os.path.join(os.path.dirname(os.path.realpath(__file__)),'./select'),
            pageFile,
            attributeId,
            start,
            end,
            str(pageSize),
            "no-stdout",
        ],
        stdout=subprocess.PIPE,
    )
    return bytes.decode(result.stdout).split('\n')[2].split(' ')[1]

def main():
    if len(sys.argv) != 5:
        print('Usage: experiment_select <csvFile> <attributeId> <start> <end>')
        sys.exit(1)
    csvFile = sys.argv[1]
    attributeId = sys.argv[2]
    start = sys.argv[3]
    end = sys.argv[4]
    
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
    ]
    sizeList = []    
    timeTookList = []
    aveRateList = []
    listResult = []
    timeAveList = []
    timeAve = 0
    numRecords = 0
    csvwriter = csv.DictWriter(sys.stdout, fieldnames=('page_size','time_took'))
    csvwriter.writeheader()
    for size in pageSizes:
        pageFile = 'heapfile' + str(size)
        subprocess.run(
        [
            os.path.join(os.path.dirname(os.path.realpath(__file__)), './csv2heapfile'),
            csvFile,
            pageFile,
            str(size),
        ],
        stdout=subprocess.PIPE,
        )
        aveRate = 0
        timeAve = 0
        timeTookList[:] = []
        numRecords = 0
        for i in range(3):
            timeTook = select(
                pageFile,
                attributeId,
                start,
                end,
                size,
            )
            csvwriter.writerow({
                'page_size': size,
                'time_took':timeTook,
            })
            
            timeTookList.append(int(timeTook))
        
        timeAve = sum(timeTookList)/3
        sizeList.append(int(size))
        timeAveList.append(timeAve)

    plt.scatter(sizeList, timeAveList)
    plt.plot(sizeList, timeAveList)
    plt.xlabel("Page Size (Bytes)")
    plt.ylabel("Select Data Time Took (ms)")
    plt.xscale('log', basex=2)
    # plt.yscale('log', basey=2)

    plt.ylim(40,150)
    plt.title('Select A-F ',fontweight='bold')
    plt.show()

if __name__ == '__main__':
    main()