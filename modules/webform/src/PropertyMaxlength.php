<?php

namespace Drupal\form_builder_webform;

/**
 * Extend maxlength property for better UX.
 */
class PropertyMaxlength extends Property {

  /**
   * Generate form-API elements for editing this property.
   */
  public function form($component, $edit, &$form_state) {
    $form = parent::form($component, $edit, $form_state);
    $form['maxlength'] += [
      '#element_validate' => ['form_validate_integer'],
      '#field_suffix' => ' ' . t('characters'),
    ];
    return $form;
  }

}
