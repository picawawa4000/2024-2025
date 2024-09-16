package org.firstinspires.ftc.teamcode;

import com.qualcomm.robotcore.eventloop.opmode.LinearOpMode;
import com.qualcomm.robotcore.eventloop.opmode.TeleOp;

@TeleOp(name="OpenCV Camera Test")
public class OpenCVCameraTest extends LinearOpMode {
    static {
        System.loadLibrary("main");
    }

    @Override
    public native void runOpMode() throws InterruptedException;
}
