<template>
  <oui-form uci-config="system">
    <oui-typed-section :title="$t('LED Configuration')" type="led" v-slot="{ s }" addremove :teasers="['name', 'sysfs', 'default', 'trigger']" v-if="sysfs.length > 0">
      <oui-form-item-input :uci-section="s" :label="$t('Name')" name="name" required/>
      <oui-form-item-select :uci-section="s" :label="$t('Led Name')" name="sysfs" :options="sysfs" required/>
      <oui-form-item-switch :uci-section="s" :label="$t('Default state')" name="default"/>
      <oui-form-item-select :uci-section="s" :label="$t('Trigger')" name="trigger" :options="triggers" initial="none" required/>
      <oui-form-item-input :uci-section="s" :label="$t('On-State Delay')" name="delayon" required rules="uinteger" depend="trigger == 'timer'" :help="$t('Time in milliseconds the LED stays on')"/>
      <oui-form-item-input :uci-section="s" :label="$t('Off-State Delay')" name="delayoff" required rules="uinteger" depend="trigger == 'timer'" :help="$t('Time in milliseconds the LED stays off')"/>
      <oui-form-item-select :uci-section="s" :label="$t('Device')" name="dev" :options="devices" depend="trigger == 'netdev'" required allow-create/>
      <oui-form-item-select :uci-section="s" :label="$t('Trigger Mode')" name="mode" :options="modes" multiple depend="trigger == 'netdev'"/>
    </oui-typed-section>
    <a-alert v-else>{{ $t('No LEDs available') }}</a-alert>
  </oui-form>
</template>

<script>
export default {
  data () {
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
    listLEDs () {
      return this.$rpc.call('system', 'led_list')
    }
  },
  created () {
    this.listLEDs().then(({ leds }) => {
      if (leds.length === 0) { return }

      leds.forEach(led => {
        this.sysfs.push(led.name)
      })

      leds[0].triggers.forEach(trigger => {
        this.triggers.push(trigger)
      })
    })

    this.$network.load().then(() => {
      this.devices = this.$network.getDevices().map(dev => dev.name).filter(name => name !== 'lo')
    })
  }
}
</script>
