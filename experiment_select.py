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
    # print(bytes.decode(result.stdout).split('\n'))
    return bytes.decode(result.stdout).split('\n')[2].split(' ')[1]

def main():
    if len(sys.argv) != 5:
        print('Usage: experiment_select <pageFile> <attributeId> <start> <end>')
        sys.exit(1)
    pageFile = sys.argv[1]
    attributeId = sys.argv[2]
    start = sys.argv[3]
    end = sys.argv[4]
    
    pageSizes = [
        1 * 2 ** 10,
        2 * 2 ** 10,   # 2 KB
        4 * 2 ** 10,   # 4 KB
        8 * 2 ** 10,   # 8 KB
        16 * 2 ** 10,  
        32 * 2 ** 10,
        64 * 2 ** 10,
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
        aveRate = 0
        timeAve = 0
        timeTookList[:] = []
        numRecords = 0
        for i in range(2):
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
        
        timeAve = sum(timeTookList)/2
        sizeList.append(int(size))
        timeAveList.append(timeAve)

    plt.scatter(sizeList, timeAveList)
    plt.plot(sizeList, timeAveList)
    plt.xlabel("Page Size (Bytes)")
    plt.ylabel("Select Data Time Took (ms)")
    plt.xscale('log', basex=2)
    plt.yscale('log', basey=2)

    plt.ylim(0,max(timeAveList)+1000)
    plt.title('Select A-F ',fontweight='bold')
    plt.show()

if __name__ == '__main__':
    main()