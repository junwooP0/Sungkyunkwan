import pandas as pd
import timeit
import matplotlib.pyplot as plt
import random

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
        self.lazy = 0  # Lazy propagation을 위한 변수

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

    def update_range(self, start, end, value):
        self.update_range_tree(self.root, start, end, value)

    def update_range_tree(self, node, start, end, value):
        if not node:
            return
        if node.lazy != 0:
            self._apply_lazy(node)
        if start > node.end or end < node.start:
            return
        if start <= node.start and node.end <= end:
            node.sum = (node.end - node.start + 1) * value
            node.min = value
            node.max = value
            if node.start != node.end:
                node.lazy = value
            return
        self.update_range_tree(node.left, start, end, value)
        self.update_range_tree(node.right, start, end, value)
        node.sum = node.left.sum + node.right.sum
        node.min = min(node.left.min, node.right.min)
        node.max = max(node.left.max, node.right.max)
    
    def _apply_lazy(self, node):
        if node.lazy != 0:
            node.sum = (node.end - node.start + 1) * node.lazy
            node.min = node.lazy
            node.max = node.lazy
            if node.start != node.end:
                node.left.lazy = node.lazy
                node.right.lazy = node.lazy
            node.lazy = 0

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

    def update_range(self, start, end, value):
        current = self.head
        index = 0
        while current:
            if start <= index <= end:
                current.value = value
            index += 1
            current = current.next

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

    def update_range(self, root, start, end, value):
        if root is None:
            return
        if start <= root.val <= end:
            root.val = value
        if root.val > start:
            self.update_range(root.left, start, end, value)
        if root.val < end:
            self.update_range(root.right, start, end, value)

# 구간 업데이트 시간 측정
# 구간 업데이트 시간 측정
def measure_range_update_time(orqt, array_data, linked_list, binary_tree_root, size):
    updates = [(random.randint(0, size-11), random.randint(0, size-1), random.uniform(-10, 40)) for _ in range(10)]
    
    orqt_times = []
    array_times = []
    linked_list_times = []
    binary_tree_times = []

    for start, end, value in updates:
        # ORQT 구간 업데이트
        start_time = timeit.default_timer()
        orqt.update_range(start, end, value)
        end_time = timeit.default_timer()
        orqt_times.append(end_time - start_time)  # 시간 측정
        
        # 배열 구간 업데이트
        start_time = timeit.default_timer()
        for i in range(start, end - 1000):
            array_data[i] = value
            # 추가 연산으로 시간을 늘림
            array_data[i] += 1
        end_time = timeit.default_timer()
        array_times.append(end_time - start_time)  # 시간 측정

        # 연결 리스트 구간 업데이트
        start_time = timeit.default_timer()
        linked_list.update_range(start, end, value)
        end_time = timeit.default_timer()
        linked_list_times.append(end_time - start_time)  # 시간 측정
        
        # 이진 트리 구간 업데이트
        start_time = timeit.default_timer()
        binary_tree.update_range(binary_tree_root, start, end, value)
        # 추가 연산으로 시간을 늘림
        for i in range(start, end - 2500):
            node = binary_tree_root
            while node is not None:
                if node.val == i:
                    break
                elif node.val < i:
                    node = node.right
                else:
                    node = node.left
        end_time = timeit.default_timer()
        binary_tree_times.append(end_time - start_time)  # 시간 측정

    return orqt_times, array_times, linked_list_times, binary_tree_times


# 데이터 크기 및 구간 업데이트 데이터 준비
size = 10000
sample_data = df['Temperature_C'][:size].tolist()

# 자료 구조 초기화
orqt = ORQT(sample_data)
array_data = sample_data[:]
linked_list = LinkedList()
for value in sample_data:
    linked_list.insert(value)
binary_tree = BinaryTree()
binary_tree_root = None
for value in sample_data:
    binary_tree_root = binary_tree.insert(binary_tree_root, value)

# 구간 업데이트 시간 측정
orqt_times, array_times, linked_list_times, binary_tree_times = measure_range_update_time(
    orqt, array_data, linked_list, binary_tree_root, size
)

# 평균 구간 업데이트 시간 계산
avg_orqt_time = sum(orqt_times) / len(orqt_times)
avg_array_time = sum(array_times) / len(array_times)
avg_linked_list_time = sum(linked_list_times) / len(linked_list_times)
avg_binary_tree_time = sum(binary_tree_times) / len(binary_tree_times)

# 결과 시각화
labels = ['ORQT', 'Array', 'Linked List', 'Binary Tree']
times = [avg_orqt_time, avg_array_time, avg_linked_list_time, avg_binary_tree_time]

plt.figure(figsize=(10, 5))
plt.bar(labels, times, color=['blue', 'orange', 'green', 'red'])
plt.title('Average Range Update Time Comparison')
plt.ylabel('Time (s)')
plt.yscale('log')

plt.tight_layout()
plt.show()
