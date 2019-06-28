<template>
  <uci-option-list :label="$t('Interface')" :name="name" uci-option="ifname" :load="load" :options="devices" :depend="depend" :multiple="multiple"></uci-option-list>
</template>

<script>
export default {
  props: {
    multiple: Boolean
  },
  data() {
    return {
      devices: []
    }
  },
  computed: {
    name() {
      if (this.multiple)
        return 'ifname_multi';
      else
        return 'ifname_single';
    },
    depend() {
      if (this.multiple)
        return 'type';
      else
        return '!type';
    }
  },
  methods: {
    load(sid, self) {
      if (this.multiple)
        return self.getUciValue(sid);

      const ifname = this.$uci.get('network', sid, 'ifname') || '';
      return ifname.split(' ')[0];
    }
  },
  created() {
    this.$network.load().then(() => {
      this.devices = this.$network.getDevices().map(d => d.name);
    });
  }
}
</script>
