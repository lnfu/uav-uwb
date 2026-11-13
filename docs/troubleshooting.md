# Troubleshooting

## The monitor shows no output

The board can stay in download mode after a reset. The firmware then does not
start.

Stop the monitor and do this command. `PORT` is the port name of the board:

```sh
esptool --port $PORT --before no-reset --after watchdog-reset chip-id
```

Wait 5 seconds, then start the monitor again.
