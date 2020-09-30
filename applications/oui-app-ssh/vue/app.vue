<template>
  <oui-form uci-config="dropbear" v-if="hasDropbear">
    <oui-typed-section :title="$t('ssh.SSH Server')" type="dropbear" v-slot="{ s }" addremove>
      <oui-form-item-select :uci-section="s" :label="$t('ssh.Interface')" name="Interface" :options="interfaces" :help="$t('ssh.InterfaceHelp')"/>
      <oui-form-item-input :uci-section="s" :label="$t('ssh.Port')" name="Port" placeholder="22" rules="port" :help="$t('ssh.PortHelp')"/>
      <oui-form-item-switch :uci-section="s" :label="$t('ssh.Password authentication')" name="PasswordAuth" initial true-value="on" false-value="off" :help="$t('ssh.Allow SSH password authentication')"/>
      <oui-form-item-switch :uci-section="s" :label="$t('ssh.Allow root logins with password')" name="RootPasswordAuth" initial true-value="on" false-value="off" :help="$t('ssh.Allow the root user to login with password')"/>
      <oui-form-item-switch :uci-section="s" :label="$t('ssh.Gateway ports')" name="GatewayPorts" true-value="on" false-value="off" :help="$t('ssh.GatewayPortsHelp')"/>
    </oui-typed-section>
  </oui-form>
</template>

<script>

export default {
  data () {
    return {
      interfaces: [
        ['', this.$t('ssh.unspecified')]
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
