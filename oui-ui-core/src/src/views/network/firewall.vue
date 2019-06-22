<template>
  <uci-form config="firewall">
    <uci-section type="defaults" :title="$t('General Settings')">
      <uci-option-switch :label="$t('Enable SYN-flood protection')" name="syn_flood"></uci-option-switch>
      <uci-option-switch :label="$t('Drop invalid packets')" name="drop_invalid"></uci-option-switch>
      <uci-option-list :label="$t('Input')" name="input" :options="targets" required></uci-option-list>
      <uci-option-list :label="$t('Output')" name="output" :options="targets" required></uci-option-list>
      <uci-option-list :label="$t('Forward')" name="forward" :options="targets" required></uci-option-list>
    </uci-section>
    <uci-section type="zone" :title="$t('Zones')" table addable :add="addZone">
      <uci-option-dummy :label="$t('Name')" name="name"></uci-option-dummy>
      <uci-option-list :label="$t('Input')" name="input" :options="targets" initial="ACCEPT" required></uci-option-list>
      <uci-option-list :label="$t('Output')" name="output" :options="targets" initial="ACCEPT" required></uci-option-list>
      <uci-option-list :label="$t('Forward')" name="forward" :options="targets" initial="REJECT" required></uci-option-list>
      <uci-option-switch :label="$t('Masquerading')" name="masq"></uci-option-switch>
      <uci-option-switch :label="$t('MSS clamping')" name="mtu_fix"></uci-option-switch>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      targets: [
        ['REJECT', this.$t('reject')],
        ['DROP', this.$t('drop')],
        ['ACCEPT', this.$t('accept')]
      ]
    }
  },
  methods: {
    addZone(self) {
      this.$prompt(this.$t('Please input a name'), this.$t('Add'), {
        inputValidator: value => {
          if (!value)
            return true;

          const sections = self.sections;
          for (let i = 0; i < sections.length; i++)
            if (sections[i].name === value)
              return this.$t('Name already used');

          return true;
        }
      }).then(({value}) => {
        if (!value)
          return;

        const sid = this.$uci.add('firewall', 'zone');
        this.$uci.set('firewall', sid, 'name', value);
        self.postAdd(sid);
      });
    }
  }
}
</script>
