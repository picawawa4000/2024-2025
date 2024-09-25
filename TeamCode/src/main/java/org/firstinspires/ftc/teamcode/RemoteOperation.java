package org.firstinspires.ftc.teamcode;

import com.qualcomm.robotcore.eventloop.opmode.LinearOpMode;
import com.qualcomm.robotcore.eventloop.opmode.TeleOp;

@TeleOp(name="Native Camera Test")
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