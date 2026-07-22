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
        
def measure_array_insertion_time(data):
    start_time = timeit.default_timer()
    array = []
    for value in data:
        array.append(value)
    end_time = timeit.default_timer()
    return end_time - start_time

class ListNode:
    def __init__(self, value=0, next=None):
        self.value = value
        self.next = next

class LinkedList:
    def __init__(self):
        self.head = None

    def insert(self, value):
        if not self.head:
            self.head = ListNode(value)
        else:
            current = self.head
            while current.next:
                current = current.next
            current.next = ListNode(value)

def measure_linked_list_insertion_time(data):
    start_time = timeit.default_timer()
    linked_list = LinkedList()
    for value in data:
        linked_list.insert(value)
    end_time = timeit.default_timer()
    return end_time - start_time

class BinaryTreeNode:
    def __init__(self, key):
        self.left = None
        self.right = None
        self.val = key

class BinaryTree:
    def insert(self, root, key):
        if root is None:
            return BinaryTreeNode(key)
        else:
            if root.val < key:
                root.right = self.insert(root.right, key)
            else:
                root.left = self.insert(root.left, key)
        return root

def measure_binary_tree_insertion_time(data):
    start_time = timeit.default_timer()
    binary_tree = BinaryTree()
    root = None
    for value in data:
        root = binary_tree.insert(root, value)
    end_time = timeit.default_timer()
    return end_time - start_time


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

# 삽입 시간 측정 및 비교
data_sizes = [100,1000,10000,100000]
orqt_insertion_times = []
array_insertion_times = []
linked_list_insertion_times = []
binary_tree_insertion_times = []

for size in data_sizes:
    sample_data = df['Temperature_C'][:size].tolist()
    orqt = ORQT(sample_data)
    
    # ORQT 삽입 시간 측정
    orqt_insertion_time = measure_insertion_time(orqt, sample_data)
    orqt_insertion_times.append(orqt_insertion_time)
    
    # Array 삽입 시간 측정
    array_insertion_time = measure_array_insertion_time(sample_data)
    array_insertion_times.append(array_insertion_time)
    
    # Linked List 삽입 시간 측정
    linked_list_insertion_time = measure_linked_list_insertion_time(sample_data)
    linked_list_insertion_times.append(linked_list_insertion_time)
    
    # Binary Tree 삽입 시간 측정
    binary_tree_insertion_time = measure_binary_tree_insertion_time(sample_data)
    binary_tree_insertion_times.append(binary_tree_insertion_time)

# 결과 시각화
plt.figure(figsize=(10, 5))

plt.plot(data_sizes, orqt_insertion_times, marker='o', label='ORQT')
plt.plot(data_sizes, array_insertion_times, marker='o', label='Array')
plt.plot(data_sizes, linked_list_insertion_times, marker='o', label='Linked List')
plt.plot(data_sizes, binary_tree_insertion_times, marker='o', label='Binary Tree')
plt.title('Insertion Time Comparison')
plt.xlabel('Data Size')
plt.ylabel('Time (s)')
plt.yscale('log')
plt.legend()

plt.show()
