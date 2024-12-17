#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Step 1: Set up Java (if not already set)
sudo apt-get update
sudo apt-get install -y openjdk-11-jdk
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64
export PATH=$JAVA_HOME/bin:$PATH

# Step 2: Install Android SDK if not already installed
if [ ! -d "$HOME/Android/Sdk" ]; then
  mkdir -p ~/Android/Sdk
  cd ~/Android/Sdk
  wget https://dl.google.com/android/repository/commandlinetools-linux-9477386_latest.zip -O commandlinetools.zip
  unzip commandlinetools.zip -d cmdline-tools
  mkdir cmdline-tools/latest
  mv cmdline-tools/cmdline-tools/* cmdline-tools/latest/
  rm -rf commandlinetools.zip
  cd -
fi

# Step 3: Set up Android SDK environment variables
export ANDROID_HOME=~/Android/Sdk
export PATH=$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH

# Step 4: Accept licenses and install required Android SDK components
yes | sdkmanager --licenses
sdkmanager "platforms;android-30" "build-tools;30.0.3"

# Step 5: Set up `local.properties` for the project
if [ ! -f "local.properties" ]; then
  echo "sdk.dir=$ANDROID_HOME" > local.properties
fi

# Step 6: Grant execute permissions to the Gradle wrapper and build the project
chmod +x ./gradlew
./gradlew :module:assembleRelease

# Step 7: Move the built artifact to a specified directory
ARTIFACT_PATH="out/magisk_module_release"
ARTIFACT_NAME="zygisk-imgui-modmenu"

if [ -d "$ARTIFACT_PATH" ]; then
  echo "Build successful. Moving artifact to target directory."
  mv "$ARTIFACT_PATH" "./${ARTIFACT_NAME}"
  echo "Artifact available at: $(pwd)/${ARTIFACT_NAME}"
else
  echo "Error: Artifact path $ARTIFACT_PATH does not exist."
  exit 1
fi