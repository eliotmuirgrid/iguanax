APP = build/Release/IguanaX.app

all: $(APP)

$(APP):
	xcodebuild -project IguanaX.xcodeproj -target IguanaX -configuration Release clean
	xcodebuild -project IguanaX.xcodeproj -target IguanaX -configuration Release

clean:
	rm -rf build