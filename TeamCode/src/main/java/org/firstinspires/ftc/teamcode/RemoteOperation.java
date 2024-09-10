package org.firstinspires.ftc.teamcode;

import com.qualcomm.robotcore.eventloop.opmode.LinearOpMode;

public class RemoteOperation extends LinearOpMode {
    static {
        System.loadLibrary("main");
    }

    public native void run();

    @Override
    public void runOpMode() throws InterruptedException {
        this.run();
    }
}