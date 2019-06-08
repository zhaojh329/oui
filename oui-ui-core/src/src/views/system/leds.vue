<template>
   <uci-form config="system">
    <uci-section title="LED Definitions" name="led" typed>
      <uci-option type="input" label="Name" name="name"></uci-option>
      <uci-option type="list" label="Led Name" name="sysfs" :options="sysfs"></uci-option>
      <uci-option type="list" label="Default state" name="default" :options="ledStates" initial="0" required></uci-option>
      <uci-option type="list" label="Trigger" name="trigger" :options="triggers"></uci-option>
      <uci-option type="input" label="On-State Delay" name="delayon" required depend="trigger == 'timer'"></uci-option>
      <uci-option type="input" label="Off-State Delay" name="delayoff" required depend="trigger == 'timer'"></uci-option>
      <uci-option type="list" label="Device" name="dev" :options="devices" depend="trigger == 'netdev'"></uci-option>
      <uci-option type="list" label="Trigger Mode" name="mode" :options="modes" multiple depend="trigger == 'netdev'"></uci-option>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      sysfs: [],
      ledStates: [
        ['0', 'off'],
        ['1', 'on']
      ],
      triggers: [],
      devices: [],
      modes: [
        ['link', 'Link On'],
        ['tx', 'Transmit'],
        ['rx', 'Receive']
      ]
    }
  },
  computed: {
  },
  methods: {
    listLEDs() {
      return this.$ubus.call('oui.system', 'led_list');
    },
    listUSBDevices() {
      return this.$ubus.call('oui.system', 'usb_list');
    }
  },
  created() {
    this.listLEDs().then(r => {
      if (r.leds.length === 0)
        return;

      r.leds.forEach(led => {
        this.sysfs.push(led.name);
      });

      r.leds[0].triggers.forEach(trigger => {
        this.triggers.push(trigger);
      });
    });

    this.$network.load().then(() => {
      this.devices = this.$network.getDevices().map(dev => dev.name);
    });
  }
}
</script>
