import pandas as pd
import timeit
import matplotlib.pyplot as plt
import psutil
import os

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

# 배열 업데이트 시간 측정
def measure_array_update_time(data, updates):
    array = data[:]
    start_time = timeit.default_timer()
    for index, value in updates:
        array[index] = value
    end_time = timeit.default_timer()
    return end_time - start_time

# 연결 리스트 업데이트 시간 측정
def measure_linked_list_update_time(linked_list, updates):
    start_time = timeit.default_timer()
    for index, value in updates:
        current = linked_list.head
        for _ in range(index):
            current = current.next
        current.value = value
    end_time = timeit.default_timer()
    return end_time - start_time

# 이진 트리 업데이트 시간 측정
def measure_binary_tree_update_time(root, updates):
    def update_node(root, key, value):
        if root is None:
            return
        if root.val == key:
            root.val = value
        elif root.val < key:
            update_node(root.right, key, value)
        else:
            update_node(root.left, key, value)

    start_time = timeit.default_timer()
    for key, value in updates:
        update_node(root, key, value)
    end_time = timeit.default_timer()
    return end_time - start_time

# ORQT 업데이트 시간 측정
def measure_orqt_update_time(orqt, updates):
    start_time = timeit.default_timer()
    for index, value in updates:
        orqt.update(index, value)
    end_time = timeit.default_timer()
    return end_time - start_time

def measure_memory_usage():
    process = psutil.Process(os.getpid())
    mem_info = process.memory_info()
    return mem_info.rss  # 메모리 사용량 (바이트 단위)

# 업데이트 시간 측정 및 비교
data_sizes = [1000, 2000, 3000, 4000, 5000]

orqt_memory_usage = []
array_memory_usage = []
linked_list_memory_usage = []
binary_tree_memory_usage = []

for size in data_sizes:
    sample_data = df['Temperature_C'][:size].tolist()
    orqt = ORQT(sample_data)
    
    # 배열 메모리 사용량 측정
    array_data = sample_data[:]
    array_memory_usage.append(measure_memory_usage())
    
    # 연결 리스트 메모리 사용량 측정
    linked_list = LinkedList()
    for value in sample_data:
        linked_list.insert(value)
    linked_list_memory_usage.append(measure_memory_usage())
    
    # 이진 트리 메모리 사용량 측정
    binary_tree = BinaryTree()
    root = None
    for value in sample_data:
        root = binary_tree.insert(root, value)
    binary_tree_memory_usage.append(measure_memory_usage())
    
    # ORQT 메모리 사용량 측정
    orqt_memory_usage.append(measure_memory_usage())

# 결과 시각화
plt.figure(figsize=(10, 5))

plt.plot(data_sizes, orqt_memory_usage, marker='o', label='ORQT')
plt.plot(data_sizes, array_memory_usage, marker='o', label='Array')
plt.plot(data_sizes, linked_list_memory_usage, marker='o', label='Linked List')
plt.plot(data_sizes, binary_tree_memory_usage, marker='o', label='Binary Tree')
plt.title('Memory Usage Comparison')
plt.xlabel('Data Size')
plt.ylabel('Memory Usage (bytes)')
plt.legend()

plt.tight_layout()
plt.show()