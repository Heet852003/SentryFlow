pipeline {
    agent any

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Build Firmware') {
            steps {
                sh '''
                  cd firmware
                  make all
                '''
            }
        }

        stage('Firmware Self-Test') {
            steps {
                sh '''
                  cd firmware
                  make test
                '''
            }
        }

        stage('Static Analysis (cppcheck)') {
            steps {
                sh '''
                  if command -v cppcheck >/dev/null 2>&1; then
                    cppcheck --enable=warning,style,performance,portability --error-exitcode=1 firmware/src firmware/include
                  else
                    echo "cppcheck not installed; skipping"
                  fi
                '''
            }
        }

        stage('Python Tooling Checks') {
            steps {
                sh '''
                  cd tools
                  python -m pip install --upgrade pip
                  pip install -r requirements.txt pytest
                  python ci_checks.py
                '''
            }
        }
    }
}

