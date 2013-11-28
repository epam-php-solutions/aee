<?php

/**
 * @file
 * Deploy Extra Log views field handlers.
 */

/**
 * Process status field from integer to string.
 */
class deploy_extra_log_handler_status_field extends views_handler_field {

  function render($values) {
    $value = _deploy_extra_log_status_to_string($values->{$this->field_alias});
    return $value;
  }

}

/**
 * Field handler to present the path to the entity.
 */

class deploy_extra_log_handler_field_entity_link extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => '', 'translatable' => TRUE);
    return $options;
  }

  function construct() {
    parent::construct();
    $this->additional_fields['entity_uuid'] = 'entity_uuid';
    $this->additional_fields['entity_type'] = 'entity_type';
  }

  function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    parent::options_form($form, $form_state);

    // The path is set by render_link function so don't allow to set it.
    $form['alter']['path'] = array('#access' => FALSE);
    $form['alter']['external'] = array('#access' => FALSE);
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {

    $entity_uuid = $this->get_value($values, 'entity_uuid');
    $entity_type = $this->get_value($values, 'entity_type');

    $ids = entity_get_id_by_uuid($entity_type, array($entity_uuid));

    if(!empty($ids)) {
      $id = reset($ids);
      $entity = entity_load_single($entity_type, $id);
      if (entity_access('view', $entity_type, $entity)) {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = "$entity_type/$id";
        $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
        return $text;
      }
    }
  }

}