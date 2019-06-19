<template>
   <uci-form config="system" v-if="sysfs.length > 0">
    <uci-section type="led" addable :teasers="['name', 'sysfs', 'default', 'trigger']">
      <uci-option-input :label="$t('Name')" name="name" required></uci-option-input>
      <uci-option-list :label="$t('Led Name')" name="sysfs" :options="sysfs" required :initial="sysfs[0]"></uci-option-list>
      <uci-option-switch :label="$t('Default state')" name="default"></uci-option-switch>
      <uci-option-list :label="$t('Trigger')" name="trigger" :options="triggers" initial="none" required></uci-option-list>
      <uci-option-input :label="$t('On-State Delay')" name="delayon" required depend="trigger == 'timer'" :description="$t('Time in milliseconds the LED stays on')"></uci-option-input>
      <uci-option-input :label="$t('Off-State Delay')" name="delayoff" required depend="trigger == 'timer'" :description="$t('Time in milliseconds the LED stays off')"></uci-option-input>
      <uci-option-list :label="$t('Device')" name="dev" :options="devices" depend="trigger == 'netdev'" allow-create></uci-option-list>
      <uci-option-list :label="$t('Trigger Mode')" name="mode" :options="modes" multiple depend="trigger == 'netdev'"></uci-option-list>
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
