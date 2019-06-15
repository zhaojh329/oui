<template>
   <uci-form config="system" v-if="sysfs.length > 0">
    <uci-section type="led" addable>
      <uci-option type="input" :label="$t('Name')" name="name" required></uci-option>
      <uci-option type="list" :label="$t('Led Name')" name="sysfs" :options="sysfs" required :initial="sysfs[0]"></uci-option>
      <uci-option type="switch" :label="$t('Default state')" name="default"></uci-option>
      <uci-option type="list" :label="$t('Trigger')" name="trigger" :options="triggers" initial="none" required></uci-option>
      <uci-option type="input" :label="$t('On-State Delay')" name="delayon" required depend="trigger == 'timer'"></uci-option>
      <uci-option type="input" :label="$t('Off-State Delay')" name="delayoff" required depend="trigger == 'timer'"></uci-option>
      <uci-option type="list" :label="$t('Device')" name="dev" :options="devices" depend="trigger == 'netdev'" allow-create></uci-option>
      <uci-option type="list" :label="$t('Trigger Mode')" name="mode" :options="modes" multiple depend="trigger == 'netdev'"></uci-option>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      sysfs: [],
      triggers: [],
      devices: [],
      modes: [
        ['link', this.$t('Link On')],
        ['tx', this.$t('Transmit')],
        ['rx', this.$t('Receive')]
      ]
    }
  },
  computed: {
  },
  methods: {
    listLEDs() {
      return this.$ubus.call('oui.system', 'led_list');
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
      this.devices = this.$network.getDevices().map(dev => dev.name).filter(name => name !== 'lo');
    });
  }
}
</script>
