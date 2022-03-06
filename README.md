# Nvidia GT520 screen fix

Software solution for random black screens with Nvidia GT520 after Windows 10 boot/wakeup.


## Usage

1. Download the latest snapshot of [nv520-screen-fix](https://github.com/AlexAltea/nv520-screen-fix/archive/master.zip).

2. Extract the contents of the .zip file and go to the *bin* folder.

3. Install the service by executing `install-service.bat` with administrator privileges.

4. Enjoy!

5. Uninstall the service by executing `uninstall-service.bat` with administrator privileges.


## Details

Under some circumstances, Windows 10 might default to "Second screen only" projection modes even if only one screen is attached. This process appears to be specific to Nvidia GT520 GPUs, presumably due to wrong information reported by the graphics card to the OS during boot/wakeup.

Since it is unlikely that either Nvidia or Microsoft correct this issue, I've created a small service that will try to correct the behaviour by querying the current display topology and reverting any `INTERNAL`/`EXTERNAL` topologies, i.e. first/second-screen only, back to a `CLONE` topology.

This is accomplished by the `ApplyTopologyFix` function which will be called:
- *During boot*: By calling it directly from the main function.
- *During wakeup*: By creating a hidden window that processes `WM_POWERBROADCAST`:`PBT_APMRESUMEAUTOMATIC` messages. This is a bit hacky, and should probably be replaced by `SERVICE_CONTROL_POWEREVENT` (see also https://stackoverflow.com/a/47943518/1114717), but I am too lazy for that.

In both cases `ApplyTopologyFix` will be constantly scanning for an unexpected topology every 3 seconds, for a total of 2 minutes.
