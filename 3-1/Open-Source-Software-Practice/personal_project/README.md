# Tower Defense Game

## 게임 설명
Tower Defense Game은 플레이어가 타워를 배치하여 적의 경로를 방어하는 전략 게임입니다. 플레이어는 타워를 업그레이드하고, 적을 물리치며, 웨이브를 견뎌내야 합니다. 각 타워는 다양한 공격 범위와 속도를 가지며, 적들은 웨이브가 진행될수록 더 강력해집니다. 플레이어는 적을 처치하면 코인을 얻고, 이를 통해 타워를 업그레이드하거나 새로운 타워를 배치할 수 있습니다. 적이 경로의 끝에 도달하면 플레이어의 목숨이 줄어들며, 목숨이 모두 소진되면 게임이 종료됩니다.

## 설치 방법

### 사전 단계
- **깃허브 클론**: [GitHub 저장소](https://github.com/JunWooP0/oss_personal_project) :
    ```bash
    git clone https://github.com/JunWooP0/oss_personal_project
    cd oss_personal_project/
    ```

- **SWIG 설치**: Box2D 패키지 설치 시 필요한 `swig`를 설치하려면 다음 명령어를 사용하세요.
  - Ubuntu 및 다른 Debian 기반 시스템:
    ```bash
    sudo apt update
    sudo apt install swig
    ```
  - CentOS 및 다른 RHEL 기반 시스템:
    ```bash
    sudo yum install swig
    ```
  - macOS:
    ```bash
    brew install swig
    ```
  - Windows :
    - 관리자 권한으로 PowerShell을 실행하고 다음 명령어를 입력하여 Chocolatey를 설치합니다:
      ```powershell
      Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
      ```
      - Chocolatey 설치가 완료되면 PowerShell을 재시작하고 다음 명령어를 입력하여 SWIG를 설치합니다:
      ```powershell
      choco install swig
      ```

- **폰트 설치**:
  - `pygame`에서 기본 폰트를 찾을 수 없는 경우, 시스템에 폰트를 설치하거나 코드에서 폰트를 명시적으로 지정해야 할 수 있습니다.
  - 예를 들어, Ubuntu에서는 다음 명령어를 사용하여 기본 폰트를 설치할 수 있습니다:
    ```bash
    sudo apt install fonts-freefont-ttf
    ```


### Windows
1. [Python3.12](https://www.python.org/downloads/) 설치:
    - 다운로드 페이지에서 설치 파일을 다운로드하고 설치 프로그램을 실행하여 Python 3.12를 설치하세요.
    - 설치 시 "Add Python to PATH" 옵션을 선택하는 것을 잊지 마세요.
    - 설치 후 명령 프롬프트(또는 PowerShell)에서 Python 버전을 확인하세요.
    ```bash
    python --version
    ```
    - pip가 설치되었는지 확인하려면 다음 명령어를 입력하세요:
    ```bash
    python -m ensurepip --upgrade
    ```
    - 가상 환경 설정 (선택 사항이지만 권장됨):
    ```bash
    python -m venv myenv
    myenv\Scripts\activate  # 가상 환경 활성화
    ```

2. [Microsoft Visual C++ Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/) 설치:
    - "Build Tools for Visual Studio 2022"를 다운로드하여 설치하세요.
    - 설치 시, "C++ build tools" 옵션을 선택하고 "MSVC v143 - VS 2022 C++ x64/x86 build tools"와 "Windows 10 SDK"를 선택하세요.

3. PowerShell에서 필요한 라이브러리 설치:
    ```bash
    pip3 install pygame
    pip3 install box2d
    ```

4. 재부팅 후 게임 실행:
    ```bash
    python main.py
    ```

### Linux
#### 옵션 1: Python 직접 설치 및 실행
1. Python3 및 pip 설치:
    ```bash
    sudo apt update
    sudo apt install python3 python3-pip
    ```

2. SWIG 설치 (필요 시):
    ```bash
    sudo apt install swig
    ```

3. 필요한 라이브러리 설치:
    ```bash
    pip3 install pygame
    pip3 install box2d
    ```


4. 폰트 설치 (필요 시):
    - Ubuntu에서는 `fontconfig` 패키지를 설치하여 `fc-list` 명령어를 사용할 수 있습니다.
    ```bash
    sudo apt install fontconfig
    ```

5. 게임 실행:
    ```bash
    python3 main.py
    ```

#### 옵션 2: Docker 사용 <sub><sup>(소리x)</sup></sub>
1. Docker 설치:
    - Ubuntu 예시:
    ```bash
    sudo apt update
    sudo apt install -y docker.io
    sudo systemctl start docker
    sudo systemctl enable docker
    ```

2. Docker 이미지 빌드:
    ```bash
    docker build -t tower-defense-game .
    ```

3. Docker 컨테이너 실행:
    ```bash
    xhost +
    docker run -it \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    tower-defense-game
    ```

### MacOS
1. Python3 및 필요한 라이브러리 설치:
    ```bash
    brew install python
    pip3 install pygame
    pip3 install box2d
    ```

2. 게임 실행:
    ```bash
    python3 main.py
    ```

### 추가 설명
- **Python 설치**: [Python 공식 문서](https://www.python.org/doc/)에서 더 많은 정보를 찾을 수 있습니다.
- **Docker 설치**: [Docker 공식 문서](https://docs.docker.com/get-docker/)에서 각 운영체제별 설치 방법을 확인할 수 있습니다.
- **xhost**: xhost 명령어는 X 서버에 접근 권한을 설정하는 명령어입니다. 이를 통해 Docker 컨테이너가 호스트의 디스플레이에 접근할 수 있도록 합니다.


## 실행 방법
게임을 실행하려면 `main.py`를 실행하세요. Docker를 사용하는 경우 위의 Linux 설치 방법을 참조하세요.

## 지원 Operating Systems
| OS       | 지원 여부 |
|----------|------------|
| Windows  | ⭕         |
| Linux    | ⭕         |
| MacOS    | ⭕         |

## 코드 설명
### 주요 클래스 및 함수
- **`class Tower`**:
  - 설명: 타워를 나타내는 클래스입니다.
  - `__init__(self, x, y)`: 타워의 초기 설정을 담당하며, 타워의 위치, 모양, 범위, 공격 속도를 초기화합니다.
  - `create_shape(self, shape)`: 주어진 모양에 따라 타워의 이미지를 생성합니다.
  - `upgrade(self)`: 타워를 업그레이드하여 모양을 변경하고, 범위 및 공격 속도를 개선합니다.
  - `attack(self, enemies, projectiles)`: 타워가 적을 공격하도록 합니다.
  
- **`class Enemy`**:
  - 설명: 적을 나타내는 클래스입니다.
  - `__init__(self, path)`: 적의 초기 설정을 담당하며, 경로, 속도, 체력을 초기화합니다.
  - `update(self)`: 적의 위치를 업데이트하고, 경로를 따라 이동합니다.
  - `take_damage(self, damage)`: 적이 피해를 입었을 때 체력을 감소시키고, 체력이 0 이하일 경우 적을 제거합니다.
  - `draw_health_bar(self, surface)`: 적의 체력 바를 그립니다.
  
- **`class Projectile`**:
  - 설명: 투사체를 나타내는 클래스입니다.
  - `__init__(self, pos, target)`: 투사체의 초기 설정을 담당하며, 위치와 목표를 초기화합니다.
  - `update(self)`: 투사체의 위치를 업데이트하고, 목표에 도달했을 때 피해를 입힙니다.
  
- **`def draw_intro_screen`**:
  - 설명: 게임의 시작 화면을 그립니다.
  - 세부사항: 타이틀, 시작 안내 문구, 게임 설명을 화면에 표시합니다.

- **`def draw_menu_button`**:
  - 설명: 메뉴 버튼을 화면에 그립니다.
  - 세부사항: 메뉴 버튼을 생성하고 화면의 우측 하단에 표시합니다.

- **`def draw_menu_screen`**:
  - 설명: 메뉴 화면을 그립니다.
  - 세부사항: 메뉴, 재개, 소리 켜기, 소리 끄기, 종료 버튼을 화면에 표시합니다.

- **`def handle_menu_screen_click(pos)`**:
  - 설명: 메뉴 화면의 버튼 클릭을 처리합니다.
  - 세부사항: 클릭한 위치에 따라 게임 상태를 변경합니다 (재개, 소리 켜기/끄기, 종료).

- **`def draw_game_over_screen`**:
  - 설명: 게임 오버 화면을 그립니다.
  - 세부사항: "GAME OVER" 텍스트와 재시작, 종료 버튼을 화면에 표시합니다.

- **`def handle_game_over_click(pos)`**:
  - 설명: 게임 오버 화면의 버튼 클릭을 처리합니다.
  - 세부사항: 클릭한 위치에 따라 게임을 재시작하거나 종료합니다.

- **`def main_game`**:
  - 설명: 게임의 메인 루프를 실행합니다.
  - 세부사항: 타워 배치, 적 생성, 웨이브 관리, 타워 공격, 화면 업데이트 등의 기능을 포함합니다.

- **`def place_tower(x, y)`**:
  - 설명: 주어진 위치에 타워를 배치하거나 업그레이드합니다.
  - 세부사항: 타워가 이미 존재하면 업그레이드하고, 그렇지 않으면 새로운 타워를 배치합니다.
  
- **`def spawn_enemy`**:
  - 설명: 새로운 적을 생성하여 경로를 따라 이동시킵니다.
  
- **`def is_docker`**:
  - 설명: 현재 환경이 Docker 컨테이너인지 확인합니다.
  - 세부사항: Docker 환경에서는 오디오를 비활성화합니다.

## 실행 예시

### 초기 화면
- 게임이 시작될 때 표시되는 초기 화면입니다.
- 게임 설명과 규칙이 화면에 표시됩니다.
![intro](https://github.com/JunWooP0/oss_personal_project/assets/163957128/420b48b6-19ec-42b2-81a1-0edf75596202)

### 게임 중간 단계
- 게임 플레이 중 타워를 배치하고 적을 물리치는 과정입니다.
![Gameplay](https://github.com/JunWooP0/oss_personal_project/assets/163957128/e1018fc2-c9b8-4fc7-bf2a-fb8f5e8fe0cc)
oP0/oss_personal_project/assets/163957128/2f999406-1305-4254-93e8-8df0a1def59b
- 웨이브 진행 상황과 현재 자원 상태를 확인할 수 있습니다.
- Menu를 눌러 다양한 기능들을 수행할 수 있습니다.
![menu](https://github.com/JunWooP0/oss_personal_project/assets/163957128/4d0ffe0a-2678-41ca-a02b-952f2793abf4)
![menu](https://github.com/JunWooP0/oss_personal_project/assets/163957128/d217a648-53d3-42bd-9f61-c35f8283f93b)

### 게임 오버 화면
- 플레이어의 라이프가 0이 되었을 때 표시되는 화면입니다.
![live0](https://github.com/JunWooP0/oss_personal_project/assets/163957128/aee3ad90-ea8e-4b74-894f-f6614aad22f5)
- "Restart" 버튼을 누르면 게임이 처음부터 다시 시작됩니다.
![end_restart](https://github.com/JunWooP0/oss_personal_project/assets/163957128/dd9c8ba6-7407-4261-816d-d1ec76d3443d)
- "Exit" 버튼을 클릭하면 게임이 종료됩니다.


## References
[1] [pybox2d](https://github.com/pybox2d/pybox2d)  
[2] [pygame](https://github.com/pygame/pygame)
[3] [freesound](https://freesound.org/)
