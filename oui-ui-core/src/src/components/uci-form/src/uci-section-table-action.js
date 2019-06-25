export default {
  name: 'UciSectionTableAction',
  props: {
    section: Object,
    sid: String
  },
  methods: {
    handleSort(up) {
      const sids = this.section.sids;
      let index = sids.indexOf(this.sid);

      if (up)
        index--;
      else
        index++;

      if (index < 0 || index === sids.length)
        return;

      const nsid = sids[index];
      this.section.swap(this.sid, nsid);
    },
    up() {
      this.handleSort(true);
    },
    down() {
      this.handleSort(false);
    },
    del() {
      this.section.del(this.sid);
    }
  },
  render() {
    if (this.section.$scopedSlots.action)
      return this.section.$scopedSlots.action({s: this.section, sid: this.sid});

    const delBtn = <el-button type="danger" size="mini" on-click={this.del}>{this.$t('Delete')}</el-button>;

    if (this.section.sortable) {
      return (
        <el-button-group>
          <el-button type="success" size="mini" on-click={this.up}>↑</el-button>
          <el-button type="success" size="mini" on-click={this.down}>↓</el-button>
          {delBtn}
        </el-button-group>
      );
    } else {
      return delBtn;
    }
  }
}
