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

