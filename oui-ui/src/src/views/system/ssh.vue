<template>
  <oui-form uci-config="dropbear" v-if="hasDropbear">
    <oui-typed-section :title="$t('SSH Server')" type="dropbear" v-slot="{ s }" addremove>
      <oui-form-item-select :uci-section="s" :label="$t('Interface')" name="Interface" :options="interfaces" :help="$t('Listen only on the given interface or, if unspecified, on all')"/>
      <oui-form-item-input :uci-section="s" :label="$t('Port')" name="Port" placeholder="22" rules="port" :help="$t('Specifies the listening port of this Dropbear instance')"/>
      <oui-form-item-switch :uci-section="s" :label="$t('Password authentication')" name="PasswordAuth" initial true-value="on" false-value="off" :help="$t('Allow SSH password authentication')"/>
      <oui-form-item-switch :uci-section="s" :label="$t('Allow root logins with password')" name="RootPasswordAuth" initial true-value="on" false-value="off" :help="$t('Allow the root user to login with password')"/>
      <oui-form-item-switch :uci-section="s" :label="$t('Gateway ports')" name="GatewayPorts" true-value="on" false-value="off" :help="$t('Allow remote hosts to connect to local SSH forwarded ports')"/>
    </oui-typed-section>
  </oui-form>
</template>

<script>

export default {
  data () {
    return {
      interfaces: [
        ['', this.$t('unspecified')]
      ],
      hasDropbear: false
    }
  },
  created () {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces()
      this.interfaces.push(...interfaces.map(item => item.name))
    })

    this.$uci.load('dropbear').then(() => {
      this.hasDropbear = true
    }).catch(() => {
      this.hasDropbear = false
    })
  }
}
</script>
