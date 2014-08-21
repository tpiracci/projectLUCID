// For an introduction to the Blank template, see the following documentation:
// http://go.microsoft.com/fwlink/?LinkId=232509
(function () {
    "use strict";

    WinJS.Binding.optimizeBindingReferences = true; //TODO FIND OUT??

    //REFERENCES
    var app = WinJS.Application;
    var activation = Windows.ApplicationModel.Activation;
    var streams = Windows.Storage.Streams;  //NOT SURE YET, PROBABLY VIDEO/STREAMING DATA?
    var kinect = WindowsPreview.Kinect;     //Connect to the Kinect SDK

    //PROG VARIABLES
    //C++ WinRT component TODO: TEST IF NECESSARY
    var coordinateMapperImageProcessor = KinectImageProcessor.CoordinateMapperHelper;
    //Kinect Sensor
    var sensor = null;
    //Reader for depth/color/body index frames
    var multiFrameSourceReader = null;
    //Containers for Frame Data
    var colorFrameBuffer = null;
    var depthFrameData = null;
    var bodyIndexFrameData = null;
    //Ref to THE Canvas and its ImageData
    var displayPixels = null;
    //Markup Objects
    var canvas = null;
    var cxt = null;


    //Handles the depth/color/body index of a frame's data from the sensor
    function reader_MultiSourceFrameArrived(args) {
        //get the multiframereference
        var multiFrameReference = args.frameReference.acquireFrame();
        if (multiFrameReference != null) {
            var colorFrame = multiFrameReference.colorFrameReference.acquireFrame();
            var depthFrame = multiFrameReference.depthFrameReference.acquireFrame();
            var bodyIndexFrame = multiFrameReference.bodyIndexFrameReference.acquireFrame();

            if ((depthFrame != null) && (colorFrame != null) && (bodyIndexFrame != null)) {
                //cope color, depth, and bodIndx stuff. put in containers?
                colorFrame.copyConvertedFrameDataToBuffer(colorFrameBuffer, kinect.ColorImageFormat.rgba);
                depthFrame.copyFrameDataToArray(depthFrameData);
                bodyIndexFrame.copyFrameDataToArray(bodyIndexFrameData);
                //call the WinRT env to process dat data and spit it into a canvas array
                if(coordinateMapperImageProcessor.processMultiFrameBufferData(depthFrameData, colorFrameData, bodyFrameData, displayPixels.data)){
                    //put img bits on dat canvass
                    cxt.clearRect(0,0,canvas.width,canvas.height);
                cxt.putImageDate(displayPixels,0,0);
                }
            }
            if(colorFrame != null){
                colorFrame.close();
            }
            if(depthFrame != null){
                depthFrame.close();
            }
            if(bodyIndexFrame != null){
                bodyIndexFrame.close();
            }
            multiFrameReference.close();
        }
    }
    //handler for sensor availability, on-ness
    function sensor_isAvailableChanged(args) {
        if(sensor.isAvailable){
            document.getElementById("statustext").innerHTML = "Totes ON, yo";
        }
        else {
            document.getElementById("statustext").innerHTML = "Kinect ain't here, yo";
        }
    }


    app.onactivated = function (args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {
                // TODO: This application has been newly launched. Initialize
                // THE APPLICATION
                //Get the sensor object
                sensor = kinect.KinectSensor.getDefault();
                //open reader for various frames
                sensor.addEventListener("isavailablechanged", sensor_IsAvailableChanged);
                //open the reader for various frames
                multiFrameSourceReader = sensor.openMultiSourceFrameReader(kinect.FrameSourceTypes.depth | kinect.FrameSourceTypes.color | kinect.FrameSourceTypes.bodyIndex);
                //wire handler for frame arrival
                multiFrameSourceReader.addEventListener("multisourceframearrived", reader_MultiSourceFrameArrived);
                //get frameDescription for color and depth frame sources
                var depthFrameDescription = sensor.depthFrameSource.frameDescription;
                var colorFrameDescription = sensor.depthFrameSource.createFrameDescription(kinect.ColorImageFormat.rgba);
                //create depth, color, and bodyIndex containters
                depthFrameData = new Uint16Array(depthFrameDescription.lengthInPixels);
                colorFrameBuffer = new streams.Buffer(colorFrameDescription.width * colorFrameDescription.height * colorFrameDescription.bytesPerPixel);
                bodyIndexFrameData = new Uint8Array(depthFrameDescription.width * depthFrameDescription.height);
                //get the ImageData object from canvas
                canvas = document.getElementById("theCanvas");
                canvas.width = depthFrameDescription.width;
                canvas.height = depthFrameDescription.height;
                cxt = canvas.getContext("2d");
                displayPixels = cxt.getImageData(0, 0, canvas.width, canvas.height)
                //open the sensor yo.
                sensor.open();
            }
            else {
                //TODO ******FIND THE POINT******
                // TODO: This application has been reactivated from suspension.
                // Restore application state here.
            }
            args.setPromise(WinJS.UI.processAll());
        }
    };
    app.oncheckpoint = function (args) {
        //TODO  ******TEST THE POINT OF THIS******
        // TODO: This application is about to be suspended. Save any state
        // that needs to persist across suspensions here. You might use the
        // WinJS.Application.sessionState object, which is automatically
        // saved and restored across suspension. If you need to complete an
        // asynchronous operation before your application is suspended, call
        // args.setPromise().
    };
    //IF STUFF IS OFF?? TODO TEST THAT
    app.onunload = function (args) {
        if (multiFrameSourceReader != null) {
            multiFrameSourceReader.close();
        }
        if (sensor != mull) {
            sensor.close();
        }
    }
    app.start();
})();
