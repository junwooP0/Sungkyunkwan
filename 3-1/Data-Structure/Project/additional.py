import pandas as pd
import time
import heapq
import matplotlib.pyplot as plt

# 데이터셋 로드
file_path = 'C:\\Users\\ppjw0\\code\\data_Structure\\project\\exercise_dataset.csv'
df = pd.read_csv(file_path)

# 필요한 열만 추출
df = df[['Exercise', 'Duration', 'Calories Burn', 'Heart Rate']]

class ExerciseRecord:
    def __init__(self, exercise_type, duration, calories, heart_rate):
        self.exercise_type = exercise_type
        self.duration = duration
        self.calories = calories
        self.heart_rate = heart_rate

    def __lt__(self, other):
        return self.calories < other.calories

    def __eq__(self, other):
        return (self.calories == other.calories and 
                self.duration == other.duration and 
                self.heart_rate == other.heart_rate and 
                self.exercise_type == other.exercise_type)

# Hybrid Multi-Attribute Tree (HMAT) 구현
class HMAT:
    def __init__(self):
        self.array = []
        self.linked_list = {}
        self.tree = {}
        self.priority_queue = []
    
    def insert(self, record):
        # 배열에 삽입
        self.array.append(record)
        
        # 연결 리스트에 삽입
        if record.exercise_type not in self.linked_list:
            self.linked_list[record.exercise_type] = []
        self.linked_list[record.exercise_type].append(record)
        
        # 트리에 삽입
        if record.duration not in self.tree:
            self.tree[record.duration] = []
        self.tree[record.duration].append(record)
        
        # 우선순위 큐에 삽입 (칼로리 소모량 기준)
        heapq.heappush(self.priority_queue, (-record.calories, record))
    
    def search(self, duration):
        return self.tree.get(duration, [])
    
    def update(self, old_record, new_record):
        # 배열에서 업데이트
        for i, record in enumerate(self.array):
            if record == old_record:
                self.array[i] = new_record
                break
        
        # 연결 리스트에서 업데이트
        if old_record.exercise_type in self.linked_list:
            for i, record in enumerate(self.linked_list[old_record.exercise_type]):
                if record == old_record:
                    self.linked_list[old_record.exercise_type][i] = new_record
                    break
        
        # 트리에서 업데이트
        if old_record.duration in self.tree:
            for i, record in enumerate(self.tree[old_record.duration]):
                if record == old_record:
                    self.tree[old_record.duration][i] = new_record
                    break
        
        # 우선순위 큐에서 업데이트 (효율적으로)
        for i in range(len(self.priority_queue)):
            if self.priority_queue[i][1] == old_record:
                self.priority_queue[i] = (-new_record.calories, new_record)
                heapq.heapify(self.priority_queue)
                break

def measure_insertion_time(tree, data):
    start_time = time.perf_counter()
    for record in data:
        tree.insert(record)
    end_time = time.perf_counter()
    return end_time - start_time

def measure_search_time(tree, durations):
    start_time = time.perf_counter()
    for duration in durations:
        tree.search(duration)
    end_time = time.perf_counter()
    return end_time - start_time

def measure_update_time(tree, data):
    start_time = time.perf_counter()
    for record in data:
        new_record = ExerciseRecord(record.exercise_type, record.duration, record.calories + 1, record.heart_rate + 1)
        tree.update(record, new_record)
    end_time = time.perf_counter()
    return end_time - start_time

# 데이터 크기 설정
data_sizes = [500, 1000, 1500, 2000, 2500, 3000]
data_sizes = [size for size in data_sizes if size <= len(df)]
insertion_times = []
search_times = []
update_times = []

for size in data_sizes:
    # 데이터 크기에 맞게 샘플링
    sampled_df = df.sample(n=size, replace=True)
    data = [ExerciseRecord(row['Exercise'], row['Duration'], row['Calories Burn'], row['Heart Rate']) for index, row in sampled_df.iterrows()]
    durations = sampled_df['Duration'].tolist()

    # 삽입, 검색, 업데이트 시간 측정
    tree = HMAT()
    insertion_time = measure_insertion_time(tree, data)
    search_time = measure_search_time(tree, durations)
    update_time = measure_update_time(tree, data)

    insertion_times.append(insertion_time)
    search_times.append(search_time)
    update_times.append(update_time)

# 그래프 출력


# 그래프 출력
plt.figure(figsize=(15, 5))

# 삽입 작업 시간 그래프
plt.subplot(1, 3, 1)
plt.plot(data_sizes, insertion_times, marker='o')
plt.title('Insertion Time')
plt.xlabel('Data Size')
plt.ylabel('Time (s)')

# 검색 작업 시간 그래프
plt.subplot(1, 3, 2)
plt.plot(data_sizes, search_times, marker='o')
plt.title('Search Time')
plt.xlabel('Data Size')
plt.ylabel('Time (s)')

# 업데이트 작업 시간 그래프
plt.subplot(1, 3, 3)
plt.plot(data_sizes, update_times, marker='o')
plt.title('Update Time')
plt.xlabel('Data Size')
plt.ylabel('Time (s)')

plt.tight_layout()
plt.show()
