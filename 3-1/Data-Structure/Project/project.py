import pandas as pd
import timeit
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

# 데이터셋 불러오기
file_path = r'C:\Users\ppjw0\code\data_Structure\project\weather_data.csv'
df = pd.read_csv(file_path)

# 날짜를 datetime 형식으로 변환
df['Date_Time'] = pd.to_datetime(df['Date_Time'])

class Node:
    def __init__(self, start, end):
        self.start = start
        self.end = end
        self.sum = 0
        self.min = float('inf')
        self.max = float('-inf')
        self.left = None
        self.right = None

class ORQT:
    def __init__(self, data):
        self.data = data
        self.root = self.build_tree(0, len(data) - 1)

    def build_tree(self, start, end):
        if start > end:
            return None
        node = Node(start, end)
        if start == end:
            node.sum = self.data[start]
            node.min = self.data[start]
            node.max = self.data[start]
        else:
            mid = (start + end) // 2
            node.left = self.build_tree(start, mid)
            node.right = self.build_tree(mid + 1, end)
            node.sum = node.left.sum + node.right.sum
            node.min = min(node.left.min, node.right.min)
            node.max = max(node.left.max, node.right.max)
        return node

    def update(self, index, value):
        self.update_tree(self.root, index, value)

    def update_tree(self, node, index, value):
        if node.start == node.end:
            node.sum = value
            node.min = value
            node.max = value
        else:
            mid = (node.start + node.end) // 2
            if index <= mid:
                self.update_tree(node.left, index, value)
            else:
                self.update_tree(node.right, index, value)
            node.sum = node.left.sum + node.right.sum
            node.min = min(node.left.min, node.right.min)
            node.max = max(node.left.max, node.right.max)

    def query(self, start, end):
        return self.query_tree(self.root, start, end)

    def query_tree(self, node, start, end):
        if node.start == start and node.end == end:
            return node.sum, node.min, node.max
        mid = (node.start + node.end) // 2
        if end <= mid:
            return self.query_tree(node.left, start, end)
        elif start > mid:
            return self.query_tree(node.right, start, end)
        else:
            left_sum, left_min, left_max = self.query_tree(node.left, start, mid)
            right_sum, right_min, right_max = self.query_tree(node.right, mid + 1, end)
            return (left_sum + right_sum, min(left_min, right_min), max(left_max, right_max))

def measure_insertion_time(orqt, data):
    start_time = timeit.default_timer()
    for i, value in enumerate(data):
        orqt.update(i, value)
    end_time = timeit.default_timer()
    return end_time - start_time

def measure_query_time(orqt, queries):
    start_time = timeit.default_timer()
    for query in queries:
        orqt.query(query[0], query[1])
    end_time = timeit.default_timer()
    return end_time - start_time

def measure_update_time(orqt, updates):
    start_time = timeit.default_timer()
    for update in updates:
        orqt.update(update[0], update[1])
    end_time = timeit.default_timer()
    return end_time - start_time

# 실험 데이터 준비
data_sizes = [1000, 2000, 3000, 4000, 5000]
insertion_times = []
query_times = []
update_times = []

for size in data_sizes:
    sample_data = df['Temperature_C'][:size].tolist()
    orqt = ORQT(sample_data)
    
    # 삽입 시간 측정
    insertion_time = measure_insertion_time(orqt, sample_data)
    insertion_times.append(insertion_time)
    
    # 검색 시간 측정
    queries = [(0, size // 2), (size // 2, size - 1)]
    query_time = measure_query_time(orqt, queries)
    query_times.append(query_time)
    
    # 업데이트 시간 측정
    updates = [(i, sample_data[i] + 1) for i in range(size)]
    update_time = measure_update_time(orqt, updates)
    update_times.append(update_time)

# 결과 시각화
plt.figure(figsize=(15, 5))

plt.subplot(1, 3, 1)
plt.plot(data_sizes, insertion_times, marker='o')
plt.title('Insertion Time')
plt.xlabel('Data Size')
plt.ylabel('Time (s)')

plt.subplot(1, 3, 2)
plt.plot(data_sizes, query_times, marker='o')
plt.title('Search Time')
plt.xlabel('Data Size')
plt.ylabel('Time (s)')
plt.gca().yaxis.set_major_formatter(ticker.ScalarFormatter(useMathText=True))
plt.gca().yaxis.set_major_formatter(ticker.FormatStrFormatter('%.2e'))

plt.subplot(1, 3, 3)
plt.plot(data_sizes, update_times, marker='o')
plt.title('Update Time')
plt.xlabel('Data Size')
plt.ylabel('Time (s)')

plt.tight_layout()
plt.show()
