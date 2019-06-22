export default {
  name: 'UciSectionTableActionUi',
  props: {
    section: Object,
    sid: String
  },
  render(h) {
    const self = this;

    if (this.section.$scopedSlots.action)
      return this.section.$scopedSlots.action({s: this.section, sid: this.sid});

    return h('el-button', {
      props: {
        type: 'danger',
        size: 'mini'
      },
      on: {
        click: () => {
          self.section.del(self.sid);
        }
      }
    }, [self.$t('Delete')]);
  }
}
