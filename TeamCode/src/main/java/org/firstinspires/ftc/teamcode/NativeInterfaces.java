package org.firstinspires.ftc.teamcode;

import android.graphics.Bitmap;

import androidx.annotation.NonNull;

import org.firstinspires.ftc.robotcore.external.function.Consumer;
import org.firstinspires.ftc.robotcore.external.function.Continuation;
import org.firstinspires.ftc.robotcore.external.function.ContinuationResult;
import org.firstinspires.ftc.robotcore.external.hardware.camera.CameraCaptureRequest;
import org.firstinspires.ftc.robotcore.external.hardware.camera.CameraCaptureSequenceId;
import org.firstinspires.ftc.robotcore.external.hardware.camera.CameraCaptureSession;
import org.firstinspires.ftc.robotcore.external.hardware.camera.CameraFrame;
import org.firstinspires.ftc.robotcore.external.stream.CameraStreamSource;

/**
 * A wrapper class for native functions. Used when a Java function requires a handler.
 * Is this a good way of doing it? No! Do I care? Even less!
 * **/

public class NativeInterfaces {
    public static class StateInterface implements CameraCaptureSession.StateCallback {
        //Stores the this pointer of the parent C++ class.
        //This is a bad thing that I do a lot.
        public long thisptr;

        StateInterface(long thisptr) {this.thisptr = thisptr;}

        @Override
        public native void onConfigured(@NonNull CameraCaptureSession session);

        @Override
        public void onClosed(@NonNull CameraCaptureSession session) {};
    }

    public static class StatusInterface implements CameraCaptureSession.StatusCallback {
        //Don't think that this does anything...
        //...for now, unimplemented
        @Override
        public void onCaptureSequenceCompleted(@NonNull CameraCaptureSession session, CameraCaptureSequenceId cameraCaptureSequenceId, long lastFrameNumber) {}
    }

    public static class CaptureInterface implements CameraCaptureSession.CaptureCallback {
        public long thisptr;

        CaptureInterface(long thisptr) {this.thisptr = thisptr;}

        @Override
        public native void onNewFrame(@NonNull CameraCaptureSession session, @NonNull CameraCaptureRequest request, @NonNull CameraFrame cameraFrame);
    }

    public static class FrameSource implements CameraStreamSource {
        @Override
        public void getFrameBitmap(Continuation<? extends Consumer<Bitmap>> continuation) {
            continuation.dispatch((ContinuationResult<Consumer<Bitmap>>)(Consumer<Bitmap> sendFrameToDriverStation) -> {
                Bitmap toSend = Bitmap.createBitmap(getFrame(), getXSize(), getYSize(), Bitmap.Config.RGB_565);
                sendFrameToDriverStation.accept(toSend);
            });
        }

        public native int[] getFrame();
        public native int getXSize();
        public native int getYSize();
    }
}
