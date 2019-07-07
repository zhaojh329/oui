<template>
  <el-button style="margin-top: 10px" type="primary" size="mini" @click="handleAdd">{{ $t('Add') }}</el-button>
</template>

<script>
export default {
  name: 'UciSectionAdd',
  props: {
    sestion: Object
  },
  methods: {
    add(name) {
      let sid;

      if (this.sestion.add) {
        sid = this.sestion.add(this.sestion);

        if (window.oui.isPromise(sid)) {
          sid.then(sid => {
            if (sid)
              this.sestion.loadAdded(sid);
          }, () => {
            /* ignore promise rejection */
          });
          return;
        }
      } else {
        sid = this.$uci.add(this.sestion.config, this.sestion.type, name);
      }

      if (sid) {
        if (this.sestion.afterAdd)
          this.sestion.afterAdd(sid, this.sestion);
        this.sestion.loadAdded(sid);
      }
    },
    handleAdd() {
      if (this.sestion.anonymous) {
        this.add();
        return;
      }

      this.$prompt(this.$t('Please input a name'), this.$t('Add'), {
        inputValidator: value => {
          if (!value || value.match(/^[a-zA-Z0-9_]+$/) === null)
            return this.$t('Must be a valid UCI identifier');

          for (let i = 0; i < this.sestion.sids.length; i++)
            if (this.sestion.sids[i] === value)
              return this.$t('Name already used');

          return true;
        }
      }).then(r => {
        this.add(r.value);
      });
    }
  }
}
</script>
